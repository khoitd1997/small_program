#!/usr/local/bin/python3

import os
import sys
import mmap
import resource

from typing import Tuple, List

# based on this datasheet: https://cdn-shop.adafruit.com/datasheets/BST-BMP280-DS001-11.pdf
testReg = {
    # NOTE: For all numeric values, they can be either dec, bin or hex
    # bin and hex need to be prefixed with 0b and 0x respectively
    #
    # base address all registers offset are based on
    "BaseAddress": "0xB00A0000",
    # How many bits to access when reading a register
    "AccessWidth": "32",
    # Endianess of the register, can be "little" or "big"
    "Endian": "little",
    #
    # example object that describes all the relevant field of the register
    # it will only print the fields you specified so you can skip over unimportant fields like
    # reserved
    # name of register and its offset from base
    "ctrl_meas:0xF4": {
        # describe bit 0-15(inclusive) of the register
        # can be just a single number to indicate this describes only a single bit
        "7:5": {
            # required property that describes the name of the bitfield
            "Name": "Temperature Oversampling Config",
            # optional, by default, use hex, can be bin, dec or hex
            "DisplayNumberBase": "hex",
            #
            # how to decode the fields
            # can be an object or a string
            "ValueDescription": {
                # key-value pair describing the meaning of specific values
                # values not described, will be described as unknown
                "0b000": "Skipped",
                "0b001": "oversampling x1",
                "0b010": "oversampling x2",
                "0b011": "oversampling x4",
                "0b0100": "oversampling x8",

                "0b101": "oversampling x16",
                "0b110": "oversampling x16",
                "0b111": "oversampling x16"
            }
        },
        "4:2": {
            "Name": "Pressure oversampling config",
            "DisplayNumberBase": "bin",
            # the other way to describe the field
            "ValueDescription": {
                # key-value pair describing the meaning of specific values
                # values not described, will be described as unknown
                "0b000": "Skipped",
                "0b001": "oversampling x1",
                "0b010": "oversampling x2",
                "0b011": "oversampling x4",
                "0b0100": "oversampling x8",
                # specify other values mean x16
                "*": "oversampling x16"
            }
        }
    },
    "status:0xF3": {
        "0": {
            "Name": "im_update",
            "DisplayNumberBase": "bin",
            # the other way to describe the field
            "ValueDescription": "Specify value of im_update[0]"
        }
    }
}

resultTableHeaders = ["Name", "RawValue", "Description"]
resultRowFormat = "{:<4}{:<40}{:<32}{:}"


def stringWithBaseToNum(str: str) -> int:
    """Convert a string that has base prefix like 0x to its int value"""
    numBase = str[0:2].lower()
    if numBase == "0x":
        return int(str, 16)
    if numBase == "0b":
        return int(str, 2)

    if not numBase.isdecimal():
        raise ValueError(f"Error: Can't convert number with base: {numBase}")

    return int(str, 10)


def printResultTableHeader() -> None:
    print(resultRowFormat.format("", *resultTableHeaders))


def printResultTableEntry(entry: Tuple) -> None:
    print(resultRowFormat.format("", *entry))


def parseRegId(regId: str) -> Tuple[str, int, str]:
    tmpArray = regId.split(":")
    if len(tmpArray) != 2:
        raise ValueError(f"Error: Invalid register ID: {regId}")

    return tmpArray[0], stringWithBaseToNum(tmpArray[1]), tmpArray[1]


# TODO: remove when done testing
testMode = 1


def getRegValue(regAbsAddr: int, accessWidthInBit: int, endian: str) -> int:
    if testMode != 1:
        # TODO: test this
        devmemFd = os.open("/dev/mem", os.O_RDONLY | os.O_SYNC)
        pageSize = resource.getpagesize()
        mappedOffset = regAbsAddr & (pageSize - 1)
        inPageOffset = regAbsAddr - mappedOffset

        with mmap.mmap(
                fileno=devmemFd,
                length=pageSize * 2,
                flags=mmap.MAP_SHARED,
                prot=mmap.PROT_READ,
                offset=mappedOffset) as mm:
            mm.seek(inPageOffset)
            bytesArray = mm.read(accessWidthInBit / 8)
            ret = int.from_bytes(bytesArray, byteorder=endian, signed=False)

        os.close(devmemFd)
    else:
        ret = (0b101 << 2) | (0b100 << 5)

    return ret


def getBitfieldShiftedVal(regVal: int, bitfieldRange: str) -> int:
    tmpArray = bitfieldRange.split(":")

    firstNum = stringWithBaseToNum(tmpArray[0])
    if len(tmpArray) == 1:
        secondNum = firstNum
    else:
        secondNum = stringWithBaseToNum(tmpArray[1])

    startBit = min(firstNum, secondNum)
    endBit = max(firstNum, secondNum)

    bitMask = 0
    for bit in range(startBit, endBit + 1):
        bitMask = bitMask | (1 << bit)

    return (regVal & bitMask) >> startBit


def printBitfield(regVal: int, bitfieldRange: str, bitfieldDescription: object) -> None:
    maskedVal = getBitfieldShiftedVal(regVal, bitfieldRange)

    if "DisplayNumberBase" not in bitfieldDescription:
        displayVal = f"0x{format(maskedVal, 'x')}"
    else:
        if bitfieldDescription["DisplayNumberBase"] == "hex":
            displayVal = f"0x{format(maskedVal, 'x')}"
        elif bitfieldDescription["DisplayNumberBase"] == "dec":
            displayVal = f"{format(maskedVal, 'd')}"
        elif bitfieldDescription["DisplayNumberBase"] == "bin":
            displayVal = f"0b{format(maskedVal, 'b')}"
        else:
            raise ValueError(
                f"Error: Invalid display base: {bitfieldDescription['DisplayNumberBase']}")

    valueDescription = bitfieldDescription['ValueDescription']
    if isinstance(valueDescription, str):
        description = valueDescription
    else:
        wildCardDescription = valueDescription.get('*')
        description = "Unknown" if wildCardDescription == None else wildCardDescription
        valueDescription.pop('*', None)

        for val, valDescription in valueDescription.items():
            if stringWithBaseToNum(val) == maskedVal:
                description = valDescription
                break

    printResultTableEntry(
        (bitfieldDescription['Name'], displayVal, description))


def getRequiredIntValue(regSetDescription: dict, key: str) -> int:
    if key not in regSetDescription:
        raise ValueError(
            f"Error: Missing {key} property")

    ret = stringWithBaseToNum(regSetDescription[key])
    del regSetDescription[key]

    return ret


def main() -> int:

    regSetDescription = testReg

    baseAddr = getRequiredIntValue(regSetDescription, "BaseAddress")
    accessWidthInBit = getRequiredIntValue(regSetDescription, "AccessWidth")

    if "Endian" not in regSetDescription:
        raise ValueError(
            "Error: Missing Endian property")

    endian = regSetDescription["Endian"]
    del regSetDescription["Endian"]

    for regId, bitfieldDescriptionList in regSetDescription.items():
        regName, regOffset, regOffsetStr = parseRegId(regId)
        print(
            f"{regName} Register(offset {regOffsetStr})")
        printResultTableHeader()

        regVal = getRegValue(baseAddr + regOffset, accessWidthInBit, endian)

        for bitfieldRange, bitfieldDescription in bitfieldDescriptionList.items():
            printBitfield(regVal, bitfieldRange, bitfieldDescription)

        print("\n")

    return 0


if __name__ == '__main__':
    sys.exit(main())  # next section explains the use of sys.exit
