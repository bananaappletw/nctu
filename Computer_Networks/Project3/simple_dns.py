"""
Simple DNS server by M.Tiger

This server only answers DNS query of A type
replies resource record of A type only if Question Name is found in a small local dictionary
otherwise, replies "not found"


please refer to the following links to aquire further DNS Protocol information
http://www.tcpipguide.com/free/t_DNSMessageHeaderandQuestionSectionFormat.htm
http://technet.microsoft.com/zh-tw/library/dd197470%28v=ws.10%29.aspx
"""
import socket

"""
parse_header parses the header of incoming DNS query
arg header : the header portion of a DNS query
returns a dictionary of parsed header information
"""
def parse_header(header):
	header_info = {}
	header_info['Transaction_ID'] = (ord(header[0:1]) << 8) + ord(header[1:2])
	header_info['QR_flag'] = (ord(header[2:3]) & 0x80) >> 7
	header_info['Opcode'] = (ord(header[2:3]) & 0x70) >> 4
	header_info['AA_flag'] = (ord(header[2:3]) & 0x04) >> 2
	header_info['TC_flag'] = (ord(header[2:3]) & 0x02) >> 1
	header_info['RD_flag'] = ord(header[2:3]) & 0x01
	header_info['RA_flag'] = (ord(header[3:4]) & 0x80) >> 7
	header_info['Response_code'] = (ord(header[3:4]) & 0x0F)
	header_info['QDCount'] = (ord(header[4:5]) << 8) + ord(header[5:6])
	header_info['ANCount'] = (ord(header[6:7]) << 8) + ord(header[7:8])
	header_info['NSCount'] = (ord(header[8:9]) << 8) + ord(header[9:10])
	header_info['ARCount'] = (ord(header[10:11]) << 8) + ord(header[11:12])
	return header_info

"""
parse_payload parses the questions of incoming DNS query
arg payload : the payload portion of a DNS query
returns a dictionary of parsed payload information
"""
def parse_payload(payload):
	payload_info = {}
	payload_info['Question_name'] = ''
	index = 0
	len = ord(payload[index])
	while len != 0:
		payload_info['Question_name'] += payload[index + 1 : index + len + 1] + '.'
		index += len + 1
		len = ord(payload[index])
	payload_info['Question_name'] = payload_info['Question_name'][:-1]
	index += 1
	payload_info['Question_type'] = (ord(payload[index]) << 8) + ord(payload[index + 1])
	index += 2
	payload_info['Question_class'] = (ord(payload[index]) << 8) + ord(payload[index + 1])
	return payload_info

"""
pack_header packs the dictionary which contains all the information and flags about the header into DNS header
arg header_info : a dictionary contains all the information and flags about the header
returns packed DNS header
"""
def pack_header(header_info):
	data = ''
	data += chr(header_info['Transaction_ID'] >> 8) + chr(header_info['Transaction_ID'] & 0xFF)
	temp = (header_info['QR_flag'] << 7) + (header_info['Opcode'] << 4) + \
	       (header_info['AA_flag'] << 2) + (header_info['TC_flag'] << 1) + (header_info['RD_flag'])
	data += chr(temp)
	temp = (header_info['RA_flag'] << 7) + (header_info['Response_code'])
	data += chr(temp)
	data += chr(header_info['QDCount'] >> 8) + chr(header_info['QDCount'] & 0xFF) + \
	        chr(header_info['ANCount'] >> 8) + chr(header_info['ANCount'] & 0xFF) + \
			chr(header_info['NSCount'] >> 8) + chr(header_info['NSCount'] & 0xFF) + \
			chr(header_info['ARCount'] >> 8) + chr(header_info['ARCount'] & 0xFF)
	return data

"""
pack_resource_record packs the mapped ip address and TLL into DNS resource record
arg ip : a string represents an IPv4 address
arg TLL : a integer represents Time-To-Live value
returns packed DNS resource record
"""
def pack_resource_record(ip, TTL):
	data = ''
	data += '\xC0\x0C'
	data += '\x00\x01'
	data += '\x00\x01'
	data += chr((TTL >> 24) & 0xFF) + chr((TTL >> 16) & 0xFF) + chr((TTL >> 8) & 0xFF) + chr(TTL & 0xFF)
	data += '\x00\x04'
	data += str.join('', map(lambda x : chr(int(x)), ip.split('.')))
	return data

def main():
	#create a UDP socket and bind to port 53 for DNS
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.bind(('', 53))
	
	#create a simple map which maps the domain name to ip address
	DNS_map = {"ptt.cc" : "140.113.168.8", \
	           "bs2.to" : "140.112.172.2"}
	
	#set default TLL
	TTL = 30
	
	while True:
		#receive data
		data, addr = sock.recvfrom(1024)
		
		#seperate the DNS header portion and parse the DNS header
		header = data[0:12]
		header_info = parse_header(header)
		
		#seperate the DNS payload portion and parse the DNS question
		payload = data[12:]
		payload_info = parse_payload(payload)
		
		#if the incoming request is a standard query and the question type is A
		if header_info['QR_flag'] == 0 and payload_info['Question_type'] == 0x01:
			print 'Got a DNS query from %s : %s' % (addr, payload_info['Question_name'])
			packet = ''
			response_rr = ''
			
			#if the question name is found in our map
			if payload_info['Question_name'] in DNS_map:
				print '\tFound! Reply : %s -> %s' % (payload_info['Question_name'], DNS_map[payload_info['Question_name']])
				
				#set the Query/Response flag to response
				header_info['QR_flag'] = 1
				
				#set Recursion Available flag to 1
				header_info['RA_flag'] = 1
				
				#set Answer Count to 1
				header_info['ANCount'] = 1
				
				#pack a resource record according to the question name
				response_rr = pack_resource_record(DNS_map[payload_info['Question_name']], TTL)
				
			#if the question name is NOT found in out map
			else:
				print '\tNot found. Reply : No such name.'
				
				#set the Query/Response flag to response
				header_info['QR_flag'] = 1
				
				#set the Response Code to "Name Error" (NXDOMAIN)
				header_info['Response_code'] = 0x03
				
			#create a response packet and send
			response_header = pack_header(header_info)
			packet += response_header	
			packet += payload
			packet += response_rr
			sock.sendto(packet, addr)

if __name__ == '__main__':
	main()