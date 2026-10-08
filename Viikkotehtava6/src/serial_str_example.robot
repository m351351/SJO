*** Settings ***
Library   String
Library   SerialLibrary

*** Variables ***
${com}   	COM12
${baud} 	115200
${board}	nRF
${seq}      000120X
${error}    80X
${seq1}     1234X
${error1}   -1X
${seq2}     1234ABX
${error2}   -2X
${seq3}     666666X
${error3}   -3X
${seq4}     174400X
${error4}   -4X

*** Test Cases ***
Connect Serial
	Log To Console  Connecting to ${board}
	Add Port  ${com}  baudrate=${baud}  encoding=ascii
	Port Should Be Open  ${com}
	Reset Input Buffer
	Reset Output Buffer

Serial Led Control
	Write Data   ${seq}   encoding=ascii 
	Log To Console   Send sequence ${seq}

	# vastaanotetaan merkkijono kunnes lopetusmerkki X (58) 
	${read} =   Read Until   terminator=58   encoding=ascii 

	# konsolille näkyviin vastaanotettu merkkijono
	Log To Console   Received ${read}
	
	# vertaillaan merkkijonoa
	Should Be Equal As Strings   ${read}    ${error}
	Log To Console   Tested ${read} is same as ${error}

	# astetta hankalampi tehdä testaus numeroina
	# koska lopetusmerkki X pitää ensin poistaa merkkijonosta
	# tai vaihtaa lopetusmerkki esim \0
	# Should Be Equal As Integers   ${read}    -1

Test1
	#kokeillaan testata vaaranlainen numero
	Write Data   ${seq3}   encoding=ascii 
	Log To Console   Send sequence ${seq3}

	# vastaanotetaan merkkijono kunnes lopetusmerkki X (58) 
	${read} =   Read Until   terminator=58   encoding=ascii 

	# konsolille näkyviin vastaanotettu merkkijono
	Log To Console   Received ${read}
	
	# vertaillaan merkkijonoa
	Should Be Equal As Strings   ${read}    ${error3}
	Log To Console   Tested ${read} is same as ${error3}

Test2
	Write Data   ${seq1}   encoding=ascii 
	Log To Console   Send sequence ${seq1}

	${read} =   Read Until   terminator=58   encoding=ascii 

	Log To Console   Received ${read}
	
	Should Be Equal As Strings   ${read}    ${error1}
	Log To Console   Tested ${read} is same as ${error1}

Test3
	Write Data   ${seq2}   encoding=ascii 
	Log To Console   Send sequence ${seq2}

	${read} =   Read Until   terminator=58   encoding=ascii 

	Log To Console   Received ${read}
	
	Should Be Equal As Strings   ${read}    ${error2}
	Log To Console   Tested ${read} is same as ${error2}


Test4
	Write Data   ${seq4}   encoding=ascii 
	Log To Console   Send sequence ${seq4}

	${read} =   Read Until   terminator=58   encoding=ascii 

	Log To Console   Received ${read}
	
	Should Be Equal As Strings   ${read}    ${error4}
	Log To Console   Tested ${read} is same as ${error4}

Disconnect Serial
	Log To Console  Disconnecting ${board}
	[TearDown]  Delete Port  ${com}


	
	
	
