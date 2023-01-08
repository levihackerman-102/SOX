import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import ssl

def send_email(recipient, subject, body):
    port = 465
    smtp_server = "smtp.gmail.com"
    sender_email = ""
    password = ""
    message = MIMEMultipart("alternative")
    message["Subject"] = subject
    message["From"] = sender_email
    message["To"] = recipient
    message.attach(MIMEText(body, "plain"))
    context = ssl.create_default_context()
    with smtplib.SMTP_SSL(smtp_server, port, context=context) as server:
        server.login(sender_email, password)
        server.sendmail(sender_email, recipient, message.as_string())

# Path: main.py

def main():
    recipient = "Borat"
    subject = "Test"
    body = "This is a test email"
    send_email(recipient, subject, body)

if __name__ == "__main__":
    main()
