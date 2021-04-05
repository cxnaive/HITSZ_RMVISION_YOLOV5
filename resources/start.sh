cd /home/ubuntu/HITSZ_RMVISION_YOLOV5/build
nohup echo ubuntu|sudo -S ./rmcv >> temp.log 2>&1 &
sleep 30s
echo ubuntu|sudo -S killall rmcv
while true
do
echo ubuntu|sudo -S ./rmcv
done
