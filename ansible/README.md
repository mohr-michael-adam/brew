# Examples on How to Run The Roles

install_brew = sudo ansible-playbook -i raspberrypi.host install_brew.yaml  -e 'url=http://192.168.50.195:5000/' -e 'beer_name="test beer 1"'