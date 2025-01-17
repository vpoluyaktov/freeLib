#include <QtGui/QApplication>

#include "../src/SmtpMime"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // This is a first demo application of the SmtpClient for Qt project


    // First we need to create an SmtpClient object
    // We will use the Gmail's smtp server (smtp.gmail.com, port 465, ssl)

    SmtpClient smtp("smtp.gmail.com", 465, SmtpClient::SslConnection);

    // We need to set the username (your email address) and password
    // for smtp authentification.

    smtp.setUser("your_email_address@gmail.com");
    smtp.setPassword("your_password");

    // Now we create a MimeMessage object. This is the email.

    MimeMessage message;

    message.setSender(new EmailAddress("your_email_address@gmail.com", "Your Name"));
    message.addRecipient(new EmailAddress("recipient@host.com", "Recipient's Name"));
    message.setSubject("SmtpClient for Qt - Demo");


    // Now add some text to the email.
    // First we create a MimeText object.

    MimeText text;

    text.setText("Hi,\nThis is a simple email message.\n");

    // Now add it to the mail

    message.addPart(&text);


    // Now we can send the mail/Volumes/Data//Volumes/Data/Progs/Lib/SmtpClient/demos/demo2Progs/Lib/SmtpClient/demos/demo3.cpp/Volumes/Data/Progs/Lib/SmtpClient/demos/demo4.cpp

    smtp.connectToHost();
    smtp.login();
    smtp.sendMail(message);
    smtp.quit();

}
