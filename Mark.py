import requests as re
num=1747
for i in range(1,num):
    i_str = str(i)
    n=str(i).zfill(6)
    file_name = i_str+ '.xml'
    f = open('Annotations/'+file_name,'a')
    f.write('''
<annotation>
	<folder>JPEGImages</folder>
	<filename>181117_''')
    f.write(n)
    f.write('''.jpeg</filename>
	<source>
		<database>Unknown</database>
	</source>
	<size>
		<width>1280</width>
		<height>720</height>
		<depth>3</depth>
	</size>
	<segmented>0</segmented>
	<object>
		<name>yujianke</name>
		<pose>Unspecified</pose>
		<truncated>0</truncated>
		<difficult>0</difficult>
		<bndbox>
			<xmin>533</xmin>
			<ymin>124</ymin>
			<xmax>850</xmax>
			<ymax>475</ymax>
		</bndbox>
	</object>
</annotation>
    ''')
    f.close()
print(n);
