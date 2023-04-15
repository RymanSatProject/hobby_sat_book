#!/bin/bash

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
