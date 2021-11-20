#!/bin/bash
powershell.exe "C:\'Program Files'\VcXsrv\vcxsrv.exe -multiwindow -clipboard -wgl -ac"
cd ../..
export LIBGL_ALWAYS_INDIRECT=1
npm run login
powershell.exe taskkill /f /im vcxsrv.exe >/dev/null