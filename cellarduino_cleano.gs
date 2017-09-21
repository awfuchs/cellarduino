function archiveWeek() {
  var ss = SpreadsheetApp.openById("1QZbcxv_Gfjh-SIt0MgurOlJsf4P5iUfyNuYgqJ87jD8");
  SpreadsheetApp.setActiveSpreadsheet(ss);
  theSheet = SpreadsheetApp.setActiveSheet(ss.getSheets()[0]);
  Logger.log(String(theSheet.getLastRow()));
  var newSheetName = Utilities.formatDate(new Date(), "PST", "'Cellar_'yyyy-MM-dd'T'HH:mm:ss");
  newSs=ss.copy(newSheetName);
  leaveHdgsAndLastRow (ss);
}

function leaveHdgsAndLastRow(ss) {
  headerRows=2; // the top two rows are the header
  SpreadsheetApp.setActiveSpreadsheet(ss)
  Logger.log(ss.getName());
  theSheet = SpreadsheetApp.setActiveSheet(ss.getSheets()[0]);
  Logger.log(theSheet.getName());
  numRows = theSheet.getLastRow();
  theSheet.deleteRows(headerRows+1, numRows-headerRows-1)
  
  
}
