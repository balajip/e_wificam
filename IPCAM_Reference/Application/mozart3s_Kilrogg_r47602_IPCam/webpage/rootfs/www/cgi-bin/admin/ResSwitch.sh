#!/bin/sh
CONF_FILE_PREFIX="/etc/conf.d/c0"
VENC_MASTER_CONF=$CONF_FILE_PREFIX"/vencmaster_conf.xml"
INDOOR_ISP_CONF=$CONF_FILE_PREFIX"/isp_template/isp_indoor.xml"

ENTRANCE_ISP_CONF=$CONF_FILE_PREFIX"/isp_template/isp_entrance.xml"
GENERAL_ISP_CONF=$CONF_FILE_PREFIX"/isp_template/isp_general.xml"
OUTDOORLOWLUX_ISP_CONF=$CONF_FILE_PREFIX"/isp_template/isp_indoorlowlux.xml"
OUTDOOR_ISP_CONF=$CONF_FILE_PREFIX"/isp_template/isp_outdoor.xml"
TEMPLATE6_ISP_CONF=$CONF_FILE_PREFIX"/isp_template/isp_template6.xml"
TEMPLATE7_ISP_CONF=$CONF_FILE_PREFIX"/isp_template/isp_template7.xml"
TEMPLATE8_ISP_CONF=$CONF_FILE_PREFIX"/isp_template/isp_template8.xml"




SWITCH_FILE=$CONF_FILE_PREFIX"/.2M3Mswitch"

if  [ -f $VENC_MASTER_CONF ];then


#when resolution has been changed from 1920x1080 to 2560x1920:
#
#Change the followings in venc_master.xml from:
#<root>
#    	<venc_master>
#	.....
#	<isp>
#		<auto_exposure>
#			<win id="0">
#				<startX>0</startX>
#				<startY>0</startY>
#				<width>1920</width>
#				<height>1080</height>
#				<weight>1</weight>
#				</win>
#		</auto_exposure>
#	</isp>
#	.....
#	</venc_master>
#</root>
#
#to
#
#<root>
#    	<venc_master>
#	.....
#	<isp>
#		<auto_exposure>
#			<win id="0">
#				<startX>0</startX>
#				<startY>0</startY>
#				<width>2560</width>
#				<height>1920</height>
#				<weight>1</weight>
#				</win>
#		</auto_exposure>
#	</isp>
#	.....
#	</venc_master>
#</root>


width=`cat $VENC_MASTER_CONF |  grep "\<resolution\>" | awk -F"<resolution>" '{print $2}' | awk -F"</resolution>" '{print $1}' |  awk -F"x" '{print $1}'`
height=`cat $VENC_MASTER_CONF |  grep "\<resolution\>" | awk -F"<resolution>" '{print $2}' | awk -F"</resolution>" '{print $1}' |  awk -F"x" '{print $2}'`
#echo "width="$width
#echo "height="$height
if [ -f $SWITCH_FILE ];then
  width_1=`cat $SWITCH_FILE | grep "width" |  awk -F":" '{print $2}'` 
  height_1=`cat $SWITCH_FILE | grep "height" |  awk -F":" '{print $2}'`
else
  touch $SWITCH_FILE
  echo "width:"$width > $SWITCH_FILE
  echo "height:"$height >> $SWITCH_FILE
  width_1="0"
  height_1="0"
fi


  
if [ "$width_1" != "$width"  -o "$height_1" != "$height" ];then

    line=`cat $VENC_MASTER_CONF | grep -n "win id=\"0\"" | awk -F':' '{print $1}'`
	sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $VENC_MASTER_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $VENC_MASTER_CONF

    #Modify the files in /etc/conf.d/c0/isp_template/* for web ui
    line=`cat $INDOOR_ISP_CONF | grep -n "win id=\"0\"" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $INDOOR_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $INDOOR_ISP_CONF
    

    line=`cat $ENTRANCE_ISP_CONF | grep -n "win id=\"0\"" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $ENTRANCE_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $ENTRANCE_ISP_CONF



    line=`cat $GENERAL_ISP_CONF | grep -n "win id=\"0\"" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $GENERAL_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $GENERAL_ISP_CONF



    line=`cat $OUTDOORLOWLUX_ISP_CONF | grep -n "win id=\"0\"" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $OUTDOORLOWLUX_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $OUTDOORLOWLUX_ISP_CONF



    line=`cat $OUTDOOR_ISP_CONF | grep -n "win id=\"0\"" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $OUTDOOR_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $OUTDOOR_ISP_CONF

    line=`cat $TEMPLATE6_ISP_CONF | grep -n "win id=\"0\"" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $TEMPLATE6_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $TEMPLATE6_ISP_CONF

    line=`cat $TEMPLATE7_ISP_CONF | grep -n "win id=\"0\"" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $TEMPLATE7_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $TEMPLATE7_ISP_CONF

    line=`cat $TEMPLATE8_ISP_CONF | grep -n "win id=\"0\"" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $TEMPLATE8_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $TEMPLATE8_ISP_CONF



	###########################################################################################


	line=`cat $VENC_MASTER_CONF | grep -n "<grid_range>" | awk -F':' '{print $1}'`
	sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $VENC_MASTER_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $VENC_MASTER_CONF

    #Modify the files in /etc/conf.d/c0/isp_template/* for web ui
    line=`cat $INDOOR_ISP_CONF | grep -n "<grid_range>" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $INDOOR_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $INDOOR_ISP_CONF
    

    line=`cat $ENTRANCE_ISP_CONF | grep -n "<grid_range>" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $ENTRANCE_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $ENTRANCE_ISP_CONF



    line=`cat $GENERAL_ISP_CONF | grep -n "<grid_range>" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $GENERAL_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $GENERAL_ISP_CONF



    line=`cat $OUTDOORLOWLUX_ISP_CONF | grep -n "<grid_range>" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $OUTDOORLOWLUX_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $OUTDOORLOWLUX_ISP_CONF



    line=`cat $OUTDOOR_ISP_CONF | grep -n "<grid_range>" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $OUTDOOR_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $OUTDOOR_ISP_CONF

    line=`cat $TEMPLATE6_ISP_CONF | grep -n "<grid_range>" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $TEMPLATE6_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $TEMPLATE6_ISP_CONF

    line=`cat $TEMPLATE7_ISP_CONF | grep -n "<grid_range>" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $TEMPLATE7_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $TEMPLATE7_ISP_CONF

    line=`cat $TEMPLATE8_ISP_CONF | grep -n "<grid_range>" | awk -F':' '{print $1}'`
    sed -i  "$((line+3))s/<width>.*<\\/width>/<width>$width<\\/width>/g"  $TEMPLATE8_ISP_CONF
    sed -i  "$((line+4))s/<height>.*<\\/height>/<height>$height<\\/height>/g"  $TEMPLATE8_ISP_CONF



    rm -f $SWITCH_FILE
    touch $SWITCH_FILE
    echo "width:"$width > $SWITCH_FILE
    echo "height:"$height >> $SWITCH_FILE
fi



else

echo "$VENC_MASTER_CONF does not exist!!"

fi