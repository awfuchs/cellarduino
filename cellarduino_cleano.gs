var SPREADSHEET_ID="1QZbcxv_Gfjh-SIt0MgurOlJsf4P5iUfyNuYgqJ87jD8";

function archiveWeek() {
  var ss = SpreadsheetApp.openById(SPREADSHEET_ID);
  SpreadsheetApp.setActiveSpreadsheet(ss);
  theSheet = SpreadsheetApp.setActiveSheet(ss.getSheets()[0]);
  Logger.log(String(theSheet.getLastRow()));
  var formattedDate = Utilities.formatDate(new Date(), "PST", "'Cellar_'yyyy-MM-dd'T'HH:mm:ss");
  newSs=ss.copy(formattedDate);
  //Logger.log(formattedDate);

}
