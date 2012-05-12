var offset = 0;
var oldcount = 0;
var lcd = lcd;
var accum = 0;
var toggle = false;

str = "";
function padstring(str) {
    for(i = str.length; i <= lcd.GetCols(); i++) str+= " ";
    return str;
}

function fooball() {
    if(lcd.GetCurrentLayout() != "layout_blank") {
        oldcount = 0;
        return;
    }
    var count = netstat.LineCount();
    var draw = false;

    if((accum++ % 2) == 0) {
        if((offset+=2) > count - 4) offset=0;
        draw = true;
    }

    if(oldcount < count) {
        offset+=count-oldcount;
        if(offset > count - 4) offset=count - 4;
        oldcount = count;
        draw = true;
    } else if(oldcount > count) {
        offset-=oldcount-count;    
        if(offset < 0) offset = 0;
        oldcount = count;
        draw = true;
    }

    if(offset > count - 4) offset = count - 4;
    if(offset < 0) offset = 0;

    var i = 1;
    var entries = new Array();
    var hash = new Array();

    entries.push(""); //offset by one for header.

    var line = offset;
    while(line++ < count && i - 1 < lcd.GetRows()) {
        var rem_address = netstat.Netstat(line, "rem_address"); // Get remote IP address
        var rem_port = netstat.Netstat(line, "rem_port"); // Get remote port
        var txt = rem_address + ":" + rem_port;
        if(hash[txt]) continue;
        hash[txt] = true;
        var st = netstat.Netstat(line, "st"); // Get connection status
        if(st == "") {
            continue;  
        }
        st = parseInt(st, 16)
        if(st != 1) { // Empty line; incremented past available lines
            continue;
        }
        entries.push(txt);
        i++;
    }
    
    lcd.SendData(0, 0, 0, padstring("NTST: cnt:" + count + " os:" + offset));

    if(draw)
    for(i = 1; i < lcd.GetRows() && i < entries.length; i++) {
        lcd.SendData(i, 0, 0, padstring(entries[i])); // Write to LCD
    }
}

function keyEvent(key) {
    if(lcd.GetCurrentLayout() != "layout_blank")
        return;
    print("Key event: " + key);

    if(key == 1) { // Decrement netstat line
        offset--;
        if(offset < 0)
            offset = 0;
    }
    if(key == 2) { // Increment netstat line
        offset++;
        if(offset >= netstat.LineCount() - 4) offset--;
    }
    if(key == 6)
        lcd.Transition(1); // Exit menu
}

if(lcd) {
    lcd["_TickUpdate()"].connect(fooball); // LCDControl provides a tick timer that we connect to function 'fooball'
    lcd["_KeypadEvent(const int)"].connect(keyEvent); // Connect keypad events to 'keyEvent'
    lcd.SetTimeout(1000);
}
