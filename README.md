MP710
=====

Приложение для изучения и работы с цифровым usb-коммутатором [MP71](http://www.olimp-z.ru/mp710) производства "Masterkit", позволяет включать и выключать порты устройства (переключатся между максимальным и минимальным значениями на выбранном порту).

Программа основана на [hidapi](http://github.com/signal11/hidapi), с реализацией на [libusb](https://github.com/libusb/libusb) v1.0 - так что для работы программы потребуются права администратора. Избежать этого можно создав для устройства свое правило монитирования: для этого нужно создать файл с произвольным именем и расширением *.rules* и разместить его по адресу: */etc/udev/rules.d*. 

> **Содержание файла:**
> ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="05df", MODE="0660", GROUP="masterkit"

После чего выполнить (подставив на место *<user>* имя вашего пользователя):
```bash
	sudo udevadm control --reload-rules
	sudo groupadd masterkit
	sudo adduser <user> masterkit
```

Описание
--------

Программа собирается посредством [QMake](http://doc.qt.io/qt-5/qmake-overview.html), как через терминал, так и посредством IDE *Qt Creator*

Клавиши 1-4 включают, соответственно, порты №1-4, клавиши 'q', 'w', 'e', 'r' - 5-6 и т.д.
