// This script has been written just to solve somebody's problem on Computer Engineers group

// Check the script host (wscript.exe or cscript.exe) used to run this script
// If the host program is not cscript.exe, then
// automatically re-launch the script using cscript.exe
var fso = WSH.CreateObject("Scripting.FileSystemObject");
var wshShell = WSH.CreateObject("WScript.Shell");
if (fso.GetBaseName(WSH.FullName).toLowerCase() != "cscript") {
    wshShell.Run('cscript "' + WSH.ScriptFullName + '"');
    WSH.Quit();
}

var WMI_Services = GetObject("winmgmts:");
with (WSH) {
    var query = "SELECT * FROM Win32_PointingDevice";
    var col = WMI_Services.ExecQuery(query);
    for (var e = new Enumerator(col); !e.atEnd(); e.moveNext()) {
        var mouse_device = e.item();
        StdOut.WriteLine("Found Mouse device '" + (mouse_device.Name) + "':");
        print_availability(mouse_device);
        StdOut.WriteLine("Device Status: " + mouse_device.Status);
        StdOut.WriteLine("Device Config Error Code: " + mouse_device.ConfigManagerErrorCode);
        StdOut.WriteLine("Error description: " + mouse_device.ErrorDescription);
        StdOut.WriteLine("Device Manufacturer: " + mouse_device.Manufacturer);
        StdOut.WriteLine("Device/Hardware ID: " + mouse_device.DeviceID);
        StdOut.WriteLine("Hardware Type: " + mouse_device.HardwareType);
        print_device_interface(mouse_device);
        StdOut.WriteBlankLines(1);
    }
    
    query = "SELECT * FROM Win32_Keyboard";
    col = WMI_Services.ExecQuery(query);
    for (var e = new Enumerator(col); !e.atEnd(); e.moveNext()) {
        var keyboard_device = e.item();
        StdOut.WriteLine("Found Keyboard device '" + (keyboard_device.Name) + "':");
        print_availability(keyboard_device);
        StdOut.WriteLine("Device Status: " + keyboard_device.Status);
        StdOut.WriteLine("Device Config Error Code: " + keyboard_device.ConfigManagerErrorCode);
        StdOut.WriteLine("Error description: " + keyboard_device.ErrorDescription);
        StdOut.WriteLine("Device/Hardware ID: " + keyboard_device.DeviceID);
        StdOut.WriteBlankLines(1);
    }    
    
    StdOut.Write("Press Enter to quit.");
    StdIn.SkipLine();
}

function print_availability(device) {
    WSH.StdOut.Write("Device Availability: ");
    
    switch (device.Availability) {
    case 1 :
        WSH.StdOut.WriteLine("Other");
        break;
    case 2 :
        WSH.StdOut.WriteLine("Unknown");
        break;
    case 3 :
        WSH.StdOut.WriteLine("Running or Full Power");
        break;
    case 4 :
        WSH.StdOut.WriteLine("Warning");
        break;
    case 5 :
        WSH.StdOut.WriteLine("In Test");
        break;
    case 6 :
        WSH.StdOut.WriteLine("Not Applicable");
        break;
    case 7 :
        WSH.StdOut.WriteLine("Power Off");
        break;
    case 8 :
        WSH.StdOut.WriteLine("Off Line");
        break;
    case 9 :
        WSH.StdOut.WriteLine("Off Duty");
        break;
    case 10 :
        WSH.StdOut.WriteLine("Degraded");
        break;
    case 11 :
        WSH.StdOut.WriteLine("Not Installed");
        break;
    case 12 :
        WSH.StdOut.WriteLine("Install Error");
        break;
    case 13 :
        WSH.StdOut.WriteLine("Power Save - Unknown\nThe device is known to be in a power save mode, but its exact status is unknown.");
        break;
    case 14 :
        WSH.StdOut.WriteLine("Power Save - Low Power Mode\nThe device is in a power save state but still functioning, and may exhibit degraded performance.");
        break;
    case 15 :
        WSH.StdOut.WriteLine("Power Save - Standby\nThe device is not functioning, but could be brought to full power quickly.");
        break;
    case 16 :
        WSH.StdOut.WriteLine("Power Cycle");
        break;
    case 17 :
        WSH.StdOut.WriteLine("Power Save - Warning\nThe device is in a warning state, though also in a power save mode.");
        break;
    }
}

function print_device_interface(device) {
    WSH.StdOut.Write("Device Interface: ");
    
    switch (device.DeviceInterface) {
    case 1 :
        WSH.StdOut.WriteLine("Other");
        break;
    case 2 :
        WSH.StdOut.WriteLine("Unknown");
        break;
    case 3 :
        WSH.StdOut.WriteLine("Serial");
        break;
    case 4 :
        WSH.StdOut.WriteLine("PS/2");
        break;
    case 5 :
        WSH.StdOut.WriteLine("Infrared");
        break;
    case 6 :
        WSH.StdOut.WriteLine("HP-HIL");
        break;
    case 7 :
        WSH.StdOut.WriteLine("Bus Mouse");
        break;
    case 8 :
        WSH.StdOut.WriteLine("ADB (Apple Desktop Bus)");
        break;
    case 160 :
        WSH.StdOut.WriteLine("Bus Mouse DB-9");
        break;
    case 161 :
        WSH.StdOut.WriteLine("Bus Mouse Micro-DIN");
        break;
    case 162 :
        WSH.StdOut.WriteLine("USB");
        break;
    }
}