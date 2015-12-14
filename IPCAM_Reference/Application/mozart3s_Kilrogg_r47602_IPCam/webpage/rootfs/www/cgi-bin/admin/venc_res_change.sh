#!/bin/sh

TEMP_FILE="/tmp/venc_res_change.sh.tmp"
max_support_resolution=""

echo "0 0 0 0" > /proc/sys/kernel/printk

sensor_type=`cat /etc/conf.d/c0/sensor_dependent.xml |  grep   "<sensor_type>" |  awk -F'<' '{ print $2 }' |  awk -F'>' '{ print $2 }'`
CHANGED=0
CONF_TAR_FILE=""

if [ "$REQUEST_METHOD" == "GET" ]; then

line_num=`cat /etc/conf.d/c0/sensor_dependent.xml | grep  -n "</max_resolution>" |  awk -F':' '{print $1}'`
max_support_resolution=`cat /etc/conf.d/c0/sensor_dependent.xml | sed -n "$((line_num-1)),$((line_num-1))p" | awk -F'<' '{ print $2 }' |  awk -F'>' '{ print $2 }'`


printf "Content-type: text/xml\r\n" 
printf "Cache-Control: no-cache\r\n\n"
printf "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"
printf "<root>"
printf "<max_support_resolution>$max_support_resolution</max_support_resolution>"
printf "</root>"
	
elif [ "$REQUEST_METHOD" == "POST" ]; then




if [ "$sensor_type" == "mt9p031" ];then
   
  if [ -f $TEMP_FILE ]; then
    rm -f $TEMP_FILE
  fi

  while read LINE; do
    echo ${LINE}   >> $TEMP_FILE  # do something with it here
  done




  #max_support_resolution=`cat $TEMP_FILE |  grep "max_support_resolution" | awk -F'<' '{ print $2 }' |  awk -F'>' '{ print $2 }'`
  max_support_resolution=`cat $TEMP_FILE  | grep "<max_support_resolution>" | awk -F'<' '{ print $3 }' | awk -F'>' '{ print $2 }'`
  rm -f $TEMP_FILE > /dev/null 2>&1

  #echo "max_support_resolution="$max_support_resolution
  if [ "$max_support_resolution" == "2560x1920" ]; then
  #echo "do 5M"
  CONF_TAR_FILE="/usr/local/firmware/default_flashfs_files_5M_9P.tar.gz"
 
 
  elif [ "$max_support_resolution" == "1920x1080" ]; then
  #echo "do 2M"
   CONF_TAR_FILE="/usr/local/firmware/default_flashfs_files_2M_9P.tar.gz"
 
 
  fi
  if [ -f $CONF_TAR_FILE ]; then

    CHANGED=1
  fi
 
  if [ $CHANGED -eq 1 ];then

      sh /usr/sbin/stopallproc > /dev/null 2>&1

      if mountpoint /etc > /dev/null 2>&1
      then
	  umount /etc > /dev/null 2>&1
      fi

      cd /mnt/flash
      rm -fr * >  /dev/null 2>&1
      tar -zxf $CONF_TAR_FILE >  /dev/null 2>&1
      /bin/sync;/bin/sync;/bin/sync
      

      printf "Content-type: text/xml\r\n" 
      printf "Cache-Control: no-cache\r\n\n"
      printf "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"
      printf "<root>"
      printf "<result>0</result>"
      printf "<content>Change to $max_support_resolution successfully. The system will reboot. Please wait...</content>"
      printf "</root>"
      sleep 1

      /usr/sbin/reboot > /dev/null 2>&1
  else

    printf "Content-type: text/xml\r\n" 
    printf "Cache-Control: no-cache\r\n\n"
    printf "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"
    printf "<root>"
    printf "<result>0</result>"
    printf "<content>$max_support_resolution is not supported. Nothing to do....</content>"
    printf "</root>"

  fi

else
  
  printf "Content-type: text/xml\r\n" 
  printf "Cache-Control: no-cache\r\n\n"
  printf "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"
  printf "<root>"
  printf "<result>0</result>"
  printf "<content>$sensor_type does not support resolution switch at the runtime.</content>"
  printf "</root>"

fi


else

printf "Content-type: text/xml\r\n" 
printf "Cache-Control: no-cache\r\n\n"
printf "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"
printf "<root>"
printf "<result>0</result>"
printf "<content>Nothing to do....</content>"
printf "</root>"


fi


