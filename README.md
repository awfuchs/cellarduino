# cellarduino

This project implements the software component of a wine cellar cooling system.

## Mechanical systems

This system uses a domestic freezer filled with chilled mass
(water, or possibly scrap aluminum or heat sinks) as a reservoir of cold; the
control system intermittently draws chilled air into the cellar to maintain
temperature stability.

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

*TO DO: The data in the spreadsheet is summarized and archived periodically
by a time-based Apps Script trigger.*


