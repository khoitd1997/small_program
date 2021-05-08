import subprocess
import os

# wmic path Win32_SerialPort where "DeviceID like 'COM1'" get DeviceID /value
# wmic path win32_pnpentity where "Caption like 'Communications Port (COM1)'" get /value

get_serial_port_caption_command = ['wmic', 'path', 'win32_pnpentity', 'where',
                                   'DeviceID like \'%%VID_0403%%\'', 'get', 'caption']
get_serial_port_caption_command_result = subprocess.run(
    get_serial_port_caption_command,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    universal_newlines=True,
    shell=True
)

serial_port_caption_list_unstripped = list(
    filter(
        lambda line: (line),
        str(get_serial_port_caption_command_result.stdout).split("\n"))
)
serial_port_caption_list = list(map(
    lambda line: (str(line).strip()),
    serial_port_caption_list_unstripped
))
serial_port_caption = serial_port_caption_list[1]
print(f"Caption: {serial_port_caption}")

get_serial_port_com_command = ['wmic', 'path', 'Win32_SerialPort',
                               'where', f'caption like \'{serial_port_caption}\'', 'get', 'DeviceID']
get_serial_port_com_command_result = subprocess.run(
    get_serial_port_com_command,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    universal_newlines=True,
    shell=True
)
print(str(get_serial_port_com_command_result.stdout))

# TODO: Change this
serial_port = "COM1"
open_serial_port_command = ['Putty.exe', '-serial',
                            f'{serial_port}', '-sercfg', '115200']
subprocess.run(
    open_serial_port_command,
    universal_newlines=True,
    env=os.environ,
    shell=True,
)
