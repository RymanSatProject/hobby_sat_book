#!/bin/bash

# Prompt
echo "*** Important! ***"
echo "This shell takes a very long time. (about 18 hours!!)"
echo -n "Do you want to run it? [Y/n]: "

read ANS
case $ANS in
  "" | [Yy]* )
    ;;
  * )
    exit 0
    ;;
esac

# swap to 2GB
sudo sed -i -e "s/^CONF_SWAPSIZE=.*/CONF_SWAPSIZE=2048/g" /etc/dphys-swapfile
sudo service dphys-swapfile restart

# Install dependent tools
#sudo apt-get update -y && sudo apt-get upgrade -y
#sudo apt-get install -y git gcc make openssl libssl-dev libbz2-dev libreadline-dev libsqlite3-dev
#sudo apt-get install -y python3-pip
sudo apt-get install -y python-pip

# for h5
sudo apt-get install -y libhdf5-serial-dev libhdf5-mpich-dev libhdf5-openmpi-dev
# for numpy
sudo apt-get install -y liblapack-dev
# for serial emulation
sudo apt-get install -y socat
# for camera timer
sudo apt-get install -y sleepenh
# for thumbnail generation
sudo apt-get install -y imagemagick

# create a project directory
PJ_ROOT=/rsp01
if [ ! -e $PJ_ROOT ]; then
    sudo mkdir -p $PJ_ROOT
fi
cd $PJ_ROOT

# Setup a python envirionment
if [ ! -e ~/.pyenv ]; then
    git clone https://github.com/pyenv/pyenv.git ~/.pyenv
fi
if [ ! -e ~/.pyenv/plugins/pyenv-virtualenv ]; then
    git clone https://github.com/pyenv/pyenv-virtualenv.git ~/.pyenv/plugins/pyenv-virtualenv
fi

# .bashrc
echo 'export PYENV_ROOT="$HOME/.pyenv"' >> ~/.bashrc
echo 'export PATH="$PYENV_ROOT/bin:$PATH"' >> ~/.bashrc
echo 'eval "$(pyenv init -)"' >> ~/.bashrc
echo 'eval "$(pyenv virtualenv-init -)"' >> ~/.bashrc

export PYENV_ROOT="$HOME/.pyenv"
export PATH="$PYENV_ROOT/bin:$PATH"
eval "$(pyenv init -)"
eval "$(pyenv virtualenv-init -)"

if [ ! -e $(pyenv root)/plugins/pyenv-update ]; then
    git clone https://github.com/pyenv/pyenv-update.git $(pyenv root)/plugins/pyenv-update
fi

# mission_image_recognition
cd $PJ_ROOT/mission_image_recognition
pyenv install 2.7.15
pyenv virtualenv 2.7.15 image_recognition

pyenv local image_recognition
source activate image_recognition
pip install --upgrade pip

## for scipy
for line in $(cat requirements.txt)
do
  pip install $line
done

## model download
wget --load-cookies /tmp/cookies.txt "https://drive.google.com/uc?export=download&confirm=$(wget --quiet --save-cookies /tmp/cookies.txt --keep-session-cookies --no-check-certificate 'https://drive.google.com/uc?export=download&id=1fP_KVUDkrC_2WKm5vrmWGHEHJZXq2UaA' -O- | sed -rn 's/.*confirm=([0-9A-Za-z_]+).*/\1\n/p')&id=1fP_KVUDkrC_2WKm5vrmWGHEHJZXq2UaA" -O /tmp/main.tflite && rm -rf /tmp/cookies.txt
wget --load-cookies /tmp/cookies.txt "https://drive.google.com/uc?export=download&confirm=$(wget --quiet --save-cookies /tmp/cookies.txt --keep-session-cookies --no-check-certificate 'https://drive.google.com/uc?export=download&id=16k4LXLy7yFlx29m7owAK38hsaKZoinhL' -O- | sed -rn 's/.*confirm=([0-9A-Za-z_]+).*/\1\n/p')&id=16k4LXLy7yFlx29m7owAK38hsaKZoinhL" -O /tmp/main.h5 && rm -rf /tmp/cookies.txt
mv /tmp/main.tflite /rsp01/mission_image_recognition/ml/models/.
mv /tmp/main.h5 /rsp01/mission_image_recognition/ml/models/.

# mission_main
cd $PJ_ROOT/mission_main
pyenv install 3.5.3

pyenv virtualenv 3.5.3 env_mission_main
source activate env_mission_main
pip3 install --upgrade pip
pip3 install -r requirements.txt

# startup
chmod +x $PJ_ROOT/start_mission.sh

# crontab
cd /tmp
crontab -l > crontab_work.txt
echo "@reboot $PJ_ROOT/start_mission.sh" >> crontab_work.txt
crontab crontab_work.txt
rm crontab_work.txt

cd ~
