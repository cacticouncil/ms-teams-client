#!/bin/bash
#.\vcxsrv.exe -multiwindow -clipboard -wgl -ac
cd ../..
export LIBGL_ALWAYS_INDIRECT=1
npm run login
#taskkill /f /im vcxsrv.exe