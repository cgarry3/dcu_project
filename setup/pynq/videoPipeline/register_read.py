from pynq import MMIO

## Register Address Map Details
IP_BASE_ADDRESS = 0x43C70000
ADDRESS_RANGE = 0x10000
ADDRESS_OFFSET = 0x10

## Access MMIO
mmio = MMIO(IP_BASE_ADDRESS, ADDRESS_RANGE)

## Read registe 10 times
for x in range(10):
    result = mmio.read(ADDRESS_OFFSET)
    print("Result is: " + str(result))
    x=x+1