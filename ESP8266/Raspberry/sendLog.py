import smtplib
import datetime

from email.MIMEMultipart import MIMEMultipart
from email.MIMEText import MIMEText
from email.MIMEBase import MIMEBase
from email import encoders

fromaddr = "measurementlogsender@gmail.com"
toaddr = "gustavoguedesab@gmail.com"
 
msg = MIMEMultipart()
 
msg['From'] = fromaddr
msg['To'] = toaddr
msg['Subject'] = "Measurement Log"
 
body = "Log until now:"
now = datetime.datetime.now()
msg.attach(MIMEText(body, 'plain'))
 
day = "%.2d"%now.day
month = "%.2d"%now.month
year = "%.2d"%now.year
filename = "Measurement:"+day+"."+month+"."+year+".txt"
attachment = open(filename, "rb")
 
part = MIMEBase('application', 'octet-stream')
part.set_payload((attachment).read())
encoders.encode_base64(part)
part.add_header('Content-Disposition', "attachment; filename= %s" % filename)
 
msg.attach(part)
 
server = smtplib.SMTP('smtp.gmail.com', 587)
server.starttls()
server.login(fromaddr, "maniot@winet")
text = msg.as_string()
server.sendmail(fromaddr, toaddr, text)
server.quit()

