Руководство пользователя.

Для того, чтобы запустить проект Вам необходимо:
1. Скачать все папки и файлы из репозитория
2. Проект разрабатывался на базе Visual Studio 2019, поэтому советуется создавать решение именно на данной версии.
3. После создания решения, перенесите в папку все скачанные файлы, а в само решение добавьте в него файл day_and_night.cpp, а так же все файлы из папки "assist"
4. Выставляем в настройках решения конфигурацию Debug и платформу х64 
5. Так же необходимо настроить сам проект в самой студии. В свойствах проекта:
	- В разделе "Каталоги VC++" в поле "Включаемые каталоги" добавляем папку "include" в нашей папке
	- В том же разделе в поле "Каталоги библиотек" добавляем папку "lib" в нашей папке
	- В разделе "Компановщик -> Ввод" в поле "Дополнительные зависимости" добавляем glfw3.lib, opengl32.lib, assimp-vc142-mtd.lib (обычно библиотека opengl32.dll установлена в Windows по умолчанию, но если она будет отсутствовать, необходимо будет дополнительно ее скачать и вставить в папку с решением)

После всех действий делаем сборку проекта. Сначала будет просто белый экран - это нормально. Моделями текстурам необходимо время на подгрузку.