#! /usr/bin/env python

from __future__ import print_function
import httplib2
import os

from apiclient import discovery, errors

from oauth2client import client
from oauth2client import tools
from oauth2client.file import Storage

theData = {}

try:
    import argparse
    flags = argparse.ArgumentParser(parents=[tools.argparser]).parse_args()
except ImportError:
    flags = None

SCOPES = 'https://www.googleapis.com/auth/spreadsheets'
CLIENT_SECRET_FILE = 'client_secret.json'
APPLICATION_NAME = 'Cellarduino data forwarder'


def get_credentials():
    home_dir = os.path.expanduser('~')
    credential_dir = os.path.join(home_dir, '.credentials')
    if not os.path.exists(credential_dir):
        os.makedirs(credential_dir)
    credential_path = os.path.join(credential_dir,
                                   'sheets.googleapis.com-python-quickstart.json')

    store = Storage(credential_path)
    credentials = store.get()
    if not credentials or credentials.invalid:
        flow = client.flow_from_clientsecrets(CLIENT_SECRET_FILE, SCOPES)
        flow.user_agent = APPLICATION_NAME
        if flags:
            credentials = tools.run_flow(flow, store, flags)
        else: # Needed only for compatibility with Python 2.6
            credentials = tools.run(flow, store)
        print('Storing credentials to ' + credential_path)
    return credentials

def log_temperature_data( date, time, goalTempSpec, goalTempAdap, tempAvg, tempMin, tempMax, duty ):
  credentials = get_credentials()
  http = credentials.authorize(httplib2.Http())
  discoveryUrl = ('https://sheets.googleapis.com/$discovery/rest?'
                  'version=v4')
  service = discovery.build('sheets', 'v4', http=http,
                            discoveryServiceUrl=discoveryUrl)

  spreadsheetId = '1QZbcxv_Gfjh-SIt0MgurOlJsf4P5iUfyNuYgqJ87jD8'
  rangeName = 'Rawdata!A:Z'

  values = [
    [
      date,
      time,
      goalTempSpec,
      goalTempAdap,
      tempAvg,
      tempMin,
      tempMax,
      duty
    ]
  ]
  body = {
    'values': values
  }
  try:
    result = service.spreadsheets().values().append(
      spreadsheetId=spreadsheetId, range=rangeName,
      valueInputOption='USER_ENTERED', body=body).execute()
  except HttpError as e:
    print("ERROR!!!  HTTP error while trying to append data:")
    print(e)

def log_alert( date, time, text ):
  credentials = get_credentials()
  http = credentials.authorize(httplib2.Http())
  discoveryUrl = ('https://sheets.googleapis.com/$discovery/rest?'
                  'version=v4')
  service = discovery.build('sheets', 'v4', http=http,
                            discoveryServiceUrl=discoveryUrl)

  spreadsheetId = '1QZbcxv_Gfjh-SIt0MgurOlJsf4P5iUfyNuYgqJ87jD8'
  rangeName = 'Alerts!A:Z'

  values = [
    [
      date,
      time,
      text
    ]
  ]
  body = {
    'values': values
  }
  try:
    result = service.spreadsheets().values().append(
      spreadsheetId=spreadsheetId, range=rangeName,
      valueInputOption='USER_ENTERED', body=body).execute()
  except HttpError as e:
    print("ERROR!!!  HTTP error while trying to append data:")
    print(e)

if __name__ == '__main__':
    main()

