// Sheet ID corresponding to the script
var kSheetID = "";

function doGet(e) {
	var result = "";

	if (!('func' in e.parameter)) {
    result = "Error: No function specified";
  } else {
		switch (e.parameter['func']) {
			case 'append':
			 	result = append(e);
				break;
			case 'recent':
				result = recent(e);
				break;
			case 'search':
				result = search(e);
				break;
			default:
				result = "Error: Unknown function";
		}
	}

	return ContentService.createTextOutput(result);
}

function append(e) {
	var sheet = SpreadsheetApp.openById(kSheetID).getActiveSheet();
	var rowData = [];

	var date = new Date();
	rowData[0] = Math.floor(date.getTime()/1000);
    rowData[2] = e.parameter['name'];

	for (var param in e.parameter) {
		if (param == 'func' || param == 'name' ) continue;
		rowData[1] = e.parameter[param];
		var row = sheet.getLastRow() + 1;
		var range = sheet.getRange(row, 1, 1, rowData.length);
		range.setValues([rowData]);
	}
	return "Ok";
}

function recent(e) {
	var sheet = SpreadsheetApp.openById(kSheetID).getActiveSheet();
	var data = sheet.getDataRange().getValues();

	var startTime = data[0][0];
	var endTime = data[data.length - 1][0];

	if ('start' in e.parameter) {
		startTime = Number(e.parameter['start']);
	}
	if ('end' in e.parameter) {
		endTime = Number(e.parameter['end']);
	}

	var devicesLastSeen = {};
	var devicesTimesSeen = {};
    var esp1 = {};
    var esp2 = {};
	for (var i = data.length - 1; i >= 0; i--) {
		if (data[i][0] > endTime) continue;
		if (data[i][0] < startTime) break;
		if (data[i][2] == e.parameter['esp1']) {
		  esp1[data[i][1]] = true;
		}
		if (data[i][2] == e.parameter['esp2']) {
		  esp2[data[i][1]] = true;
		}
		if (!('esp1' in e.parameter) || data[i][2] == e.parameter['esp1'] ||
			(('esp2' in e.parameter) && data[i][2] == e.parameter['esp2'])) {
		  if (!(data[i][1] in devicesLastSeen)) {
				devicesLastSeen[data[i][1]] = data[i][0];
				devicesTimesSeen[data[i][1]] = 1;
			} else {
				devicesTimesSeen[data[i][1]] += 1;
			}
		}
	}

	var result = "";
	if ('esp1' in e.parameter && 'esp2' in e.parameter) {
		for (var device in esp1) {
			if (device in esp2) {
				result += " > <font color='#4FFFB0'>" + device.toString() + "</font> seen " + devicesTimesSeen[device] + " times,<br> > last seen: " + timestampToDate(devicesLastSeen[device]) + "<br>\n";
			}
		}
	} else {
		for (var device in devicesLastSeen) {
			result += " > <font color='#4FFFB0'>" + device.toString() + "</font> seen " + devicesTimesSeen[device] + " times,<br> > last seen: " + timestampToDate(devicesLastSeen[device]) + "<br>\n";
		}
	}

	return result;
}

function search(e) {
	var sheet = SpreadsheetApp.openById(kSheetID).getActiveSheet();
	var data = sheet.getDataRange().getValues();

	var startTime = data[0][0];
	if ('start' in e.parameter) {
		startTime = Number(e.parameter['start']);
	}

	var endTime = data[data.length - 1][0];
	if ('end' in e.parameter) {
		endTime = Number(e.parameter['end']);
	}

	var mac = "";
	if ('mac' in e.parameter) {
		mac = e.parameter['mac'];
	} else {
		return " > Error: No address specified"
	}

	var result = "";
	for (var i = data.length - 1; i >= 0; i--) {
		if (data[i][0] > endTime) continue;
		if (data[i][0] < startTime) break;
		if (data[i][1] == mac) {
			result += " > " + timestampToDate(data[i][0]) + " in " + data[i][2] + "<br>\n";
		}
	}

	return result;
}

function timestampToDate(timestamp) {
	var date = new Date(timestamp*1000);

	var hours = "0" + date.getHours();
	var minutes = "0" + date.getMinutes();
	var seconds = "0" + date.getSeconds();

	var day = "0" + date.getDate();
	var month = "0" + (date.getMonth() + 1);
	var year = date.getFullYear();

	var fmtDate = day.substr(-2) + '/' + month.substr(-2) + '/' + year;
	var fmtTime = hours.substr(-2) + ':' + minutes.substr(-2) + ':' + seconds.substr(-2);

	return fmtTime + ' (' + fmtDate + ')';
}