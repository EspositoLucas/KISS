#/bin/bash

rm -rf so-commons-library

git clone https://github.com/sisoputnfrba/so-commons-library

if cd so-commons-library && make install; then
    cd && rm -rf so-commons-library install-commons.sh
    echo -e "\033[1;32m so-commons-library installed successfully\033[0m"
else
    echo -e "\033[1;31m so-commons-library installation failed\033[0m"
fi
