echo Executing NeXTTool to upload main.rxe...
export SUDO_ASKPASS="/home/robo/nxtOSEK/pw.sh"
sudo /home/robo/bricxcc/NeXTTool /COM=usb -download=main.rxe
sudo /home/robo/bricxcc/NeXTTool /COM=usb -listfiles=main.rxe
echo NeXTTool is terminated.
