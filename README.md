# cellarduino

This project implements the software component of a wine cellar cooling system.

## Mechanical systems

This system uses a domestic freezer filled with chilled mass (water bottles, or
possibly scrap aluminum or heat sinks) as a reservoir of cold; the
control system intermittently draws chilled air into the cellar to maintain
temperature stability.

![cellarduino](https://user-images.githubusercontent.com/5116094/30630801-ee0aa56c-9d96-11e7-9ad0-f2e42f6eff60.jpg)

About the machinery:

We're using a 3.5 cu ft top-loading chest freezer and the room being
cooled is a closet of about 75 cu ft.

The top of the freezer is replaced with 2" insulating board, with two
ports in the top, connected by insulated duct to the wine closet.

## Hardware and software

An Arduino reads a DHT22 temperature sensor, turns a fan on/off to maintain
temperature, and sends log information to the serial port.

A laptop captures the log information from the Arduino, aggregates it, and
records the log information in a Google Sheets spreadsheet.

![](https://user-images.githubusercontent.com/5116094/30629921-21f33078-9d93-11e7-8fad-adea6ea4f3e6.png)

*TO DO: The data in the spreadsheet is summarized and archived periodically
by a time-based Apps Script trigger.*

![](https://docs.google.com/spreadsheets/d/e/2PACX-1vREUiSFU5l1Lzm_zp1cpYfo3B8JV4Mn41EFr_9RZDIjNZDpZcTGQsGl5bASn3xf5UZYY8FwSj9jsuK-/pubhtml?gid=388140311&single=true)

### Arduino source code

The Arduino code is contained in [DHT_handler.py](/path/to/be/determined)

### Laptop/concentrator source code
The laptop code runs in Python 2.7, and is contained in the following files:

* [report-temp.py](/path/to/be/determined) -- The  serial receiving, basic counters, logic, and message formatting code.
* [logtemps.py](/path/to/be/determined) -- Connects to Google Sheets API.

## Planned improvements

* General code health cleanup.
* Google Apps Script periodic trigger to clean up spreadsheet and archive data summaries.
* Adaptive goal temperature to relax constraints when fan duty cycle exceeds a defined threshold. 
  (Allow the cellar to warm slightly during heat wave, rather than melt the freezer.)
