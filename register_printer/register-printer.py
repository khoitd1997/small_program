#!/usr/bin/python3

import os
import sys
import mmap
import resource

from typing import Tuple

class COLORS:
    BLACK = "\033[30m"
    RED = "\033[31m"
    GREEN = "\033[32m"
    YELLOW = "\033[33m"
    BLUE = "\033[34m"
    MAGENTA = "\033[35m"
    CYAN = "\033[36m"
    WHITE = "\033[37m"

    RESET = "\033[0m"

# NOTE: Modify this table based on your hardware
# below example is based on UART0 of this: https://www.xilinx.com/html_docs/registers/ug1087/ug1087-zynq-ultrascale-registers.html
registerSpecification = {
    # NOTE: For all numeric values, they can be either dec, bin or hex
    # bin and hex need to be prefixed with 0b and 0x respectively
    #
    # base address all registers offset are based on
    "BaseAddress": "0x00FF000000",
    # How many bits to access when reading a register
    "AccessWidth": "32",
    # Endianess of the register, can be "little" or "big"
    "Endian": "little",
    #
    # example object that describes all the relevant field of the register
    # it will only print the fields you specified so you can skip over unimportant fields like
    # reserved

    # the key is the name of register and its offset from base
    # the value is an object describing the different fields in a register
    "mode_reg0:0x04": {
        "5:3": {
            "Name": "Parity type select",
            # optional, by default, use hex, can be bin, dec or hex
            "DisplayNumberBase": "bin",

            # there are two ways to describe a field, with a list of enum
            # or with a string
            # the below demonstrates the enum case
            "ValueDescription": {
                "0b000": "even parity",
                "0b001": "odd parity",
                "0b010": "forced to 0 parity (space)",
                "0b011": "forced to 1 parity (mark)",
                "*": "no parity",
            }
        },
        "2:1": {
            "Name": "Character length select",
            "DisplayNumberBase": "bin",
            "ValueDescription": {
                "0b11": "6 bits",
                "0b10": "7 bits",
                "*": "8 bits",
            }
        },
        "0": {
            "Name": "Clock source select",
            "DisplayNumberBase": "bin",
            "ValueDescription": {
                "0b000": "UART_REF_CLK",
                "0b001": "UART_REF_CLK/8",
            }
        }
    },
    "Rcvr_timeout_reg0:0x1C": {
        "7:0": {
            "Name": "Receiver timeout value",
            "DisplayNumberBase": "hex",

            # this shows the non enum case where this string is always displayed
            # regardless of the register's value, useful for open ended parameter
            "ValueDescription": "Specify Value of Receiver Timeout"
        }
    }
}

resultRowFormat = "{:<4}{:<50}{:<10}{:}"


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

def printResultTableEntry(entry: Tuple) -> None:
    print(resultRowFormat.format("", *entry))


def parseRegId(regId: str) -> Tuple[str, int, str]:
    tmpArray = regId.split(":")
    if len(tmpArray) != 2:
        raise ValueError(f"Error: Invalid register ID: {regId}")

    return tmpArray[0], stringWithBaseToNum(tmpArray[1]), tmpArray[1]

def getRegValue(regAbsAddr: int, accessWidthInBit: int, endian: str) -> int:
    devmemFd = os.open("/dev/mem", os.O_RDONLY | os.O_SYNC)
    pageSize = resource.getpagesize()
    mappedOffset = regAbsAddr & (~(pageSize - 1))
    inPageOffset = regAbsAddr - mappedOffset

    with mmap.mmap(
            fileno=devmemFd,
            length=pageSize * 2,
            flags=mmap.MAP_SHARED,
            prot=mmap.PROT_READ,
            offset=mappedOffset) as mm:
        mm.seek(inPageOffset)
        bytesArray = mm.read(int(accessWidthInBit / 8))
        ret = int.from_bytes(bytesArray, byteorder=endian, signed=False)

    os.close(devmemFd)

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
        (f"{COLORS.CYAN}{bitfieldDescription['Name'] + ' Field'}{COLORS.RESET}", displayVal, f"{COLORS.BLUE}{description}{COLORS.RESET}"))


def getRequiredIntValue(regSetDescription: dict, key: str) -> int:
    if key not in regSetDescription:
        raise ValueError(
            f"Error: Missing {key} property")

    ret = stringWithBaseToNum(regSetDescription[key])
    del regSetDescription[key]

    return ret


def main() -> int:
    baseAddr = getRequiredIntValue(registerSpecification, "BaseAddress")
    accessWidthInBit = getRequiredIntValue(registerSpecification, "AccessWidth")

    if "Endian" not in registerSpecification:
        raise ValueError(
            "Error: Missing Endian property")

    endian = registerSpecification["Endian"]
    del registerSpecification["Endian"]

    for regId, bitfieldDescriptionList in registerSpecification.items():
        regName, regOffset, regOffsetStr = parseRegId(regId)
        print(
            f"{COLORS.YELLOW}{regName} Register(offset {regOffsetStr}){COLORS.RESET}")

        regVal = getRegValue(baseAddr + regOffset, accessWidthInBit, endian)

        for bitfieldRange, bitfieldDescription in bitfieldDescriptionList.items():
            printBitfield(regVal, bitfieldRange, bitfieldDescription)

        print("\n")

    return 0


if __name__ == '__main__':
    sys.exit(main())  # next section explains the use of sys.exit
