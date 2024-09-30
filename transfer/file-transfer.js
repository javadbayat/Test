// Check the script host (wscript.exe or cscript.exe) used to run this script
// If the host program is not cscript.exe, then
// automatically re-launch the script using cscript.exe
var fso = WSH.CreateObject("Scripting.FileSystemObject");
var wshShell = WSH.CreateObject("WScript.Shell");
if (fso.GetBaseName(WSH.FullName).toLowerCase() != "cscript") {
    wshShell.Run('cscript "' + WSH.ScriptFullName + '"');
    WSH.Quit();
}

with (WSH) {
    var shell = CreateObject("Shell.Application");
    
    StdOut.WriteLine("To connect to your smartphone, enter your Device Token:");
    var input = StdIn.ReadLine();
    if (!input) // The user has entered nothing (an empty line)
        Quit(); // So the operation is considered to be canceled
    
    StdOut.WriteLine("Connecting to the Android device...");
    var ipAddress = ipAddressFromToken(input);
    var port = 8007;
    
    shell.Open("ftp://" + ipAddress + ":" + port + "/");
}

function ipAddressFromToken(token) {
    var rawIP = parseInt(token, 36);
    if (isNaN(rawIP))
        throw new Error("Invalid device token");
    
    var bytes = new Array(4);
    bytes[0] = (rawIP & 0xFF000000) >> 24;
    bytes[1] = (rawIP & 0x00FF0000) >> 16;
    bytes[2] = (rawIP & 0x0000FF00) >> 8;
    bytes[3] = (rawIP & 0x000000FF);
    
    return bytes.join(".");
}