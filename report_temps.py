import serial, time, numpy
from logtemps import log_temperature_data, log_alert


PORT = "/dev/tty.usbmodem621"
BAUD = 9600

RPT_INTERVAL = 600 # seconds
LOG_TO_CLOUD = True

# incoming data field definitions

FAN = 0
CELSIUS = 2
HUMIDITY = 4
GOAL_TEMP = 6

ALERT = 0
ALERT_TYPE = 1

ser = serial.Serial(PORT, BAUD)


# Initialize all the counters and flags

lastReport = 0
prevMinute = ''
fanOns     = 0
fanOffs    = 0
fanOnOffs  = 0
maxTemp    = 0
minTemp    = 99
allTemp    = []
DC         = 0
goalTempSpec ='19.00'
goalTempAdap ='19.00'


while 1:
  # --- get the input ---
  raw = ser.readline()
  data = str(raw).split()

  # all processing happens only when there are input lines
  if len(data) >1:

    # --- set up the current time info ---
    rawTime = time.time()
    theTime = time.localtime(rawTime)
    displayDate = time.strftime("%b %d, %Y")
    displayTime = time.strftime("%H:%M")
    currentMinute  = time.strftime("%M")

    if data[ALERT] == "ALERT":
      if data[ALERT_TYPE] == "RESTARTED":
        print("%s ALERT! The controller restarted" % displayTime)
        if LOG_TO_CLOUD:
          log_alert( displayDate, displayTime, "Controller restarted")
      elif data[ALERT_TYPE] == "ADAPT_UP":
        print("%s ALERT! The goal temperature has been raised to %s Celsius."
        % (displayTime, data[CELSIUS]))
        if LOG_TO_CLOUD:
          log_alert( displayDate, displayTime, "Goal temperature raised to %s C" % data[CELSIUS])
      elif data[ALERT_TYPE] == "ADAPT_DN":
        print("%s ALERT! The goal temperature has been lowered to %s Celsius."
        % (displayTime, data[CELSIUS]))
        if LOG_TO_CLOUD:
          log_alert( displayDate, displayTime, "Goal temperature lowered to %s C" % data[CELSIUS])
    else:
      print("%s Temp: %s C, Goal: %s Fan %s (prev duty cycle %.0f%%)"
            % (displayTime, data[CELSIUS], data[GOAL_TEMP], data[FAN], DC*100))
      if data[FAN] == "OPERATING":
        fanOns += 1
      else:
        fanOffs += 1
      # --- keep the min, max, and samples for avgt ---
      theTemp = float(data[CELSIUS])
      maxTemp = max(maxTemp, theTemp)
      minTemp = min(minTemp, theTemp)
      allTemp.append(theTemp)
  
      modMinutes = int(currentMinute) % int(RPT_INTERVAL/60)
      #if rawTime > lastReport+RPT_INTERVAL:
      if (prevMinute != currentMinute) and (modMinutes == 0):
        prevMinute = currentMinute
        print("Reporting")
        fanOnOffs = fanOns + fanOffs
        if fanOnOffs > 0:
          DC = fanOns / float(fanOnOffs)
        else:
          DC = 0
  
        avgTemp = numpy.mean(allTemp)
  
        #print("%s %s Temps: min %s, max %s, avg %s, fan duty cycle %.0f%%" % (displayDate, displayTime, minTemp, maxTemp, avgTemp, DC*100))
  
        if LOG_TO_CLOUD:
          log_temperature_data( displayDate, displayTime, goalTempSpec, goalTempAdap, avgTemp, minTemp, maxTemp, DC )

        # --- reset accumulators ---
        lastReport = rawTime 
        fanOns  = 0
        fanOffs = 0
        fanOnOffs = 0
        maxTemp = theTemp
        minTemp = theTemp
        allTemp = []


