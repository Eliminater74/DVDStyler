[Setup]
AppName=DVDStyler
AppVerName=DVDStyler v2.8
AppPublisherURL=http://www.dvdstyler.org
AppSupportURL=http://www.dvdstyler.org
AppUpdatesURL=http://www.dvdstyler.org
DefaultDirName={pf}\DVDStyler
DefaultGroupName=DVDStyler
OutputBaseFilename=DVDStylerPortable-2.8-win32
Compression=lzma
SolidCompression=yes
UninstallDisplayIcon={app}\bin\DVDStyler.exe
PrivilegesRequired=none
WizardSmallImageFile=dvdstyler.bmp
AllowNoIcons=yes
;LicenseFile=..\COPYING
InfoBeforeFile=Info.rtf

[Languages]
Name: "ar"; MessagesFile: "Languages\Arabic.isl"
Name: "ca"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "cs"; MessagesFile: "compiler:Languages\Czech.isl"  
Name: "da"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"
Name: "el"; MessagesFile: "Languages\Greek.isl"
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "es_ar"; MessagesFile: "Languages\SpanishArg.isl"
Name: "eu"; MessagesFile: "Languages\Basque.isl"
Name: "fi"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "fr"; MessagesFile: "compiler:Languages\French.isl"
Name: "hu"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "hy"; MessagesFile: "Languages\Armenian.islu"
Name: "it"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "ja"; MessagesFile: "compiler:Languages\Japanese.isl"
Name: "ko"; MessagesFile: "Languages\Korean.isl"
Name: "mk"; MessagesFile: "Languages\Macedonian.isl"
Name: "nb"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "nl"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "pl"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "pt"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "pt_BR"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "ro"; MessagesFile: "Languages\Romanian.isl"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "sk"; MessagesFile: "Languages\Slovak.isl"
Name: "sl"; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "sr"; MessagesFile: "Languages\Serbian.isl"
Name: "sv"; MessagesFile: "Languages\Swedish.isl"
Name: "tr"; MessagesFile: "Languages\Turkish.isl"
Name: "uk"; MessagesFile: "Languages\Ukrainian.isl"
Name: "vi"; MessagesFile: "Languages\Vietnamese.isl"
Name: "zh_CN"; MessagesFile: "Languages\ChineseSimp.isl"
Name: "zh_TW"; MessagesFile: "Languages\ChineseTrad.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "..\src\*.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\src\*.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\src\fonts.conf"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "..\backgrounds\*.jpg"; DestDir: "{app}\backgrounds"; Flags: ignoreversion
Source: "..\backgrounds\*.png"; DestDir: "{app}\backgrounds"; Flags: ignoreversion
Source: "..\backgrounds\License.txt"; DestDir: "{app}\backgrounds"; Flags: ignoreversion
Source: "..\buttons\*.xml"; DestDir: "{app}\buttons"; Flags: ignoreversion
Source: "..\buttons\deprecated\*.xml"; DestDir: "{app}\buttons\deprecated"; Flags: ignoreversion
Source: "..\buttons\*.lst"; DestDir: "{app}\buttons"; Flags: ignoreversion
Source: "..\objects\*.xml"; DestDir: "{app}\objects"; Flags: ignoreversion
Source: "..\objects\deprecated\*.xml"; DestDir: "{app}\objects\deprecated"; Flags: ignoreversion
Source: "..\templates\Basic\*.png"; DestDir: "{app}\templates\Basic"; Flags: ignoreversion
Source: "..\templates\Basic\*.dvdt"; DestDir: "{app}\templates\Basic"; Flags: ignoreversion
Source: "..\templates\Birthday\*.png"; DestDir: "{app}\templates\Birthday"; Flags: ignoreversion
Source: "..\templates\Birthday\*.dvdt"; DestDir: "{app}\templates\Birthday"; Flags: ignoreversion
Source: "..\templates\Christmas\*.png"; DestDir: "{app}\templates\Christmas"; Flags: ignoreversion
Source: "..\templates\Christmas\*.dvdt"; DestDir: "{app}\templates\Christmas"; Flags: ignoreversion
Source: "..\templates\Miscellaneous\*.png"; DestDir: "{app}\templates\Miscellaneous"; Flags: ignoreversion
Source: "..\templates\Miscellaneous\*.dvdt"; DestDir: "{app}\templates\Miscellaneous"; Flags: ignoreversion
Source: "..\templates\Nature\*.png"; DestDir: "{app}\templates\Nature"; Flags: ignoreversion
Source: "..\templates\Nature\*.dvdt"; DestDir: "{app}\templates\Nature"; Flags: ignoreversion
Source: "..\templates\Seasons\*.png"; DestDir: "{app}\templates\Seasons"; Flags: ignoreversion
Source: "..\templates\Seasons\*.dvdt"; DestDir: "{app}\templates\Seasons"; Flags: ignoreversion
Source: "..\templates\Travel\*.png"; DestDir: "{app}\templates\Travel"; Flags: ignoreversion
Source: "..\templates\Travel\*.dvdt"; DestDir: "{app}\templates\Travel"; Flags: ignoreversion
Source: "..\templates\Wedding\*.png"; DestDir: "{app}\templates\Wedding"; Flags: ignoreversion
Source: "..\templates\Wedding\*.dvdt"; DestDir: "{app}\templates\Wedding"; Flags: ignoreversion
Source: "..\transitions\*.xml"; DestDir: "{app}\transitions"; Flags: ignoreversion                
Source: "..\transitions\*.png"; DestDir: "{app}\transitions"; Flags: ignoreversion
Source: "..\data\*.mpg"; DestDir: "{app}\data"; Flags: ignoreversion
Source: "..\data\*.rgb"; DestDir: "{app}\data"; Flags: ignoreversion
Source: "..\docs\help_*.zip"; DestDir: "{app}\docs"; Flags: ignoreversion
Source: "..\COPYING"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\ChangeLog"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\README"; DestDir: "{app}"; Flags: ignoreversion

Source: "..\locale\ar\*"; DestDir: "{app}\locale\ar"; Flags: ignoreversion
Source: "..\locale\ca\*"; DestDir: "{app}\locale\ca"; Flags: ignoreversion
Source: "..\locale\cs\*"; DestDir: "{app}\locale\cs"; Flags: ignoreversion
Source: "..\locale\da\*"; DestDir: "{app}\locale\da"; Flags: ignoreversion
Source: "..\locale\de\*"; DestDir: "{app}\locale\de"; Flags: ignoreversion
Source: "..\locale\el\*"; DestDir: "{app}\locale\el"; Flags: ignoreversion
Source: "..\locale\es\*"; DestDir: "{app}\locale\es"; Flags: ignoreversion
Source: "..\locale\es_ar\*"; DestDir: "{app}\locale\es_ar"; Flags: ignoreversion
Source: "..\locale\eu\*"; DestDir: "{app}\locale\eu"; Flags: ignoreversion
Source: "..\locale\fi\*"; DestDir: "{app}\locale\fi"; Flags: ignoreversion
Source: "..\locale\fr\*"; DestDir: "{app}\locale\fr"; Flags: ignoreversion
Source: "..\locale\hu\*"; DestDir: "{app}\locale\hu"; Flags: ignoreversion
Source: "..\locale\hy\*"; DestDir: "{app}\locale\hy"; Flags: ignoreversion
Source: "..\locale\it\*"; DestDir: "{app}\locale\it"; Flags: ignoreversion
Source: "..\locale\ja\*"; DestDir: "{app}\locale\ja"; Flags: ignoreversion
Source: "..\locale\ko\*"; DestDir: "{app}\locale\ko"; Flags: ignoreversion
Source: "..\locale\mk\*"; DestDir: "{app}\locale\mk"; Flags: ignoreversion
Source: "..\locale\nb\*"; DestDir: "{app}\locale\nb"; Flags: ignoreversion
Source: "..\locale\nl\*"; DestDir: "{app}\locale\nl"; Flags: ignoreversion
Source: "..\locale\pl\*"; DestDir: "{app}\locale\pl"; Flags: ignoreversion
Source: "..\locale\pt\*"; DestDir: "{app}\locale\pt"; Flags: ignoreversion
Source: "..\locale\pt_BR\*"; DestDir: "{app}\locale\pt_BR"; Flags: ignoreversion
Source: "..\locale\ro\*"; DestDir: "{app}\locale\ro"; Flags: ignoreversion
Source: "..\locale\ru\*"; DestDir: "{app}\locale\ru"; Flags: ignoreversion
Source: "..\locale\sk\*"; DestDir: "{app}\locale\sk"; Flags: ignoreversion
Source: "..\locale\sl\*"; DestDir: "{app}\locale\sl"; Flags: ignoreversion
Source: "..\locale\sr\*"; DestDir: "{app}\locale\sr"; Flags: ignoreversion
Source: "..\locale\sv\*"; DestDir: "{app}\locale\sv"; Flags: ignoreversion
Source: "..\locale\tr\*"; DestDir: "{app}\locale\tr"; Flags: ignoreversion
Source: "..\locale\uk\*"; DestDir: "{app}\locale\uk"; Flags: ignoreversion
Source: "..\locale\uz\*"; DestDir: "{app}\locale\uz"; Flags: ignoreversion
Source: "..\locale\vi\*"; DestDir: "{app}\locale\vi"; Flags: ignoreversion
Source: "..\locale\zh_CN\*"; DestDir: "{app}\locale\zh_CN"; Flags: ignoreversion
Source: "..\locale\zh_TW\*"; DestDir: "{app}\locale\zh_TW"; Flags: ignoreversion

[Icons]
Name: "{group}\DVDStyler"; Filename: "{app}\bin\DVDStyler.exe"
Name: "{group}\{cm:ProgramOnTheWeb,DVDStyler}"; Filename: "http://www.dvdstyler.org/"
Name: "{group}\{cm:UninstallProgram,DVDStyler}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\DVDStyler"; Filename: "{app}\bin\DVDStyler.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\DVDStyler"; Filename: "{app}\bin\DVDStyler.exe"; Tasks: quicklaunchicon

[INI]
Filename: "{app}\DVDStyler.ini"; Section: "Interface"; Flags: uninsdeletesection
Filename: "{app}\DVDStyler.ini"; Section: "Interface"; Key: "LanguageCode"; String: "{language}"
Filename: "{app}\DVDStyler.ini"; Section: "Interface"; Key: "ClearThumbnailCache"; String: "1"

[UninstallDelete]
Type: files; Name: "{app}\DVDStyler.ini"

[Run]
Filename: "{app}\bin\DVDStyler.exe"; Description: "{cm:LaunchProgram,DVDStyler}"; Flags: nowait postinstall skipifsilent



