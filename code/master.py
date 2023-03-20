import smbus
import time
import Jetson.GPIO as GPIO
import max6675
from datetime import datetime, timezone, timedelta
import csv
import requests

sck = 7
cs = 24
so = 21

GPIO.setmode(GPIO.BOARD)

max6675.set_pin(cs, sck, so, 1)

bus = smbus.SMBus(1)  # 1 indicates /dev/i2c-1

address = 0x08  # Arduino Uno address

data = [0x01, 0x02, 0x03, 0x04]  # example data to send

tz = timezone(timedelta(hours = 7))

while True:
    now = datetime.now()
    timestamp = datetime.now()
    date = datetime.now(tz=tz)
    # print(date.isoformat(sep = " "))
    url = "https://blynk.cloud/external/api/update?token=qUmCRBq6jQknDnYZ36opiFUHuLzPYT3a&v10="+str(date.isoformat(sep = " "))
    # print(url)
    requests.get(url)
    minute = now.minute

    if(minute/10 < 1):
        list_of_min = [0]
        list_of_min.extend([int(x) & 0xFF for x in str(minute)])
    else:
        list_of_min = [int(x) & 0xFF for x in str(minute)]

    temp = max6675.read_temp(cs)
    url = "https://blynk.cloud/external/api/update?token=qUmCRBq6jQknDnYZ36opiFUHuLzPYT3a&v0="+str(temp)
    requests.get(url)
    tempa = int(temp*100)
    list_of_digits = [int(x) & 0xFF for x in str(tempa)]
    list_of_digits.extend([0,0])
    list_of_digits.extend(list_of_min)
    print("Data sent: ", list_of_digits)

    bus.write_i2c_block_data(address, 0, list_of_digits)

    with open('temperature_log.csv', mode='a', newline='') as file:
        writer = csv.writer(file)

        # Write header row to CSV file
        # writer.writerow(['Time', 'Temperature'])

        # Loop to log temperature data

        timestamp = datetime.now()
        temperature = temp

        writer.writerow([timestamp, temperature])


    time.sleep(20)
