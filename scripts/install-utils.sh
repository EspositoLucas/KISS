#/bin/bash


if cd /home/utnso/shared/TP/tp-2022-1c-Ubunteam/utils && make install; then
    cd && rm -rf /home/utnso/shared/TP/tp-2022-1c-Ubunteam/utils install-commons.sh
    echo -e "\033[1;32m utils  installed successfully\033[0m"
else
    echo -e "\033[1;31m utils installation failed\033[0m"
fi
