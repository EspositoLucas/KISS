#/bin/bash

TP_FOLDER_PATH=< /home/utnso/shared/TP/tp-2022-1c-Ubunteam >
IP_USER=<VM USER>

# Check if folder not found
if [ ! -d "$TP_FOLDER_PATH" ]; then
    echo "\033[1;31m Folder not found: $TP_FOLDER_PATH\033[0m"
    exit 1
fi

# Check if ips.txt not found
if [ ! -f "ips.txt" ]; then
    echo "\033[1;31m File not found: ips.txt\033[0m"
    exit 1
fi

# Check if ips.txt is empty
if [ ! -s "ips.txt" ]; then
    echo "\033[1;31m File is empty: ips.txt\033[0m"
    exit 1
fi

for ip in $(cat ips.txt); do
    echo "Copying tp to $ip"
    # check error copying folder path
    if scp -r $TP_FOLDER_PATH $IP_USER@$ip:~/; then
        echo -e "\033[1;32m tp copied successfully to $ip\033[0m"
    else
        echo -e "\033[1;31m tp copy failed to $ip\033[0m"
        exit 1
    fi
    scp -r install-commons.sh $IP_USER@$ip:~/
    echo "\033[1;32m $ip is ready to run the install-commons.sh script\033[0m"
done
