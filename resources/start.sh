pass_word="Chen3xun27"
home_dir="andychen"
cd /home/${home_dir}/HITSZ_RMVISION_YOLOV5/build
echo ${pass_word}|sudo -S nohup ./start_loop "./rmcv" >temp.txt 2>&1 &
sleep 45s
echo ${pass_word}|sudo -S killall start_loop
echo ${pass_word}|sudo -S killall rmcv 
echo ${pass_word}|sudo -S ./start_loop "./rmcv"