import serial, time

# PORT = "/dev/tty.usbserial-DA01R30E"
PORT = "/dev/tty.usbmodem621"
BAUD = 9600

RPT_INTERVAL = 300 # seconds
DC_WINDOW = 3 # intervals

# incoming data field definitions

FAN = 0
CELSIUS = 2

ser = serial.Serial(PORT, BAUD)


# Initialize all the counters and flags

lastReport = 0
fanOns     = 0
fanOffs    = 0
fanOnOffs  = 0


while 1:
  # --- get the input ---
  raw = ser.readline()
  data = str(raw).split()

  # all processing happens only when there are input lines
  if len(data) >2:
    # --- set up the current time info ---
    rawTime = time.time()
    theTime = time.localtime(rawTime)
    displayTime = time.strftime("%Y.%m.%d %H:%M")
    if data[FAN] != "IDLE":
      fanOns += 1
    else:
      fanOffs += 1
    if rawTime > lastReport+RPT_INTERVAL:
      fanOnOffs = fanOns + fanOffs
      if fanOnOffs > 0:
        DC = fanOns / float(fanOnOffs)
        # print(displayTime + " Temp: "+ str(data[CELSIUS]) +" C, fan is "+ data[FAN] +", duty-cycle "+DC)
        print("%s Temp: %s C, fan %s (prev 15 min duty cycle %.0f%%)" % (displayTime, data[CELSIUS], data[FAN], DC*100))
      else:
        print(displayTime + " Temp: "+ str(data[CELSIUS]) +" C, fan is "+ data[FAN] +", duty-cycle (n/a)")
      #print( fanOns, fanOffs, fanOnOffs, DC )
      lastReport = rawTime 
      fanOns  = 0
      fanOffs = 0
      fanOnOffs = 0
