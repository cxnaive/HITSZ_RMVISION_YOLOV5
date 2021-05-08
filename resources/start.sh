pass_word="ubuntu"
home_dir="ubuntu"
cd /home/${home_dir}/HITSZ_RMVISION_YOLOV5/build
echo ${pass_word}|sudo -S nohup ./start_loop "./rmcv" >warm_up.txt 2>&1 &
sleep 45s
echo ${pass_word}|sudo -S killall start_loop
echo ${pass_word}|sudo -S killall rmcv 
echo ${pass_word}|sudo -S ./start_loop "./rmcv"