<p style="font: 30px Verdana sans-serif;" align="center">
Заголовки
</p>

<table style="font: 20px Verdana sans-serif; text-align: center;">

<tr> 
	<td>Функционал</td>
	<td>Статус</td>
	<td>Комментарий</td>
</tr>

<tr>
	<td>1. Accept-Charsets</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Точно узнать кодировку сложно,<br>
		поэтому смысла нет.
	</td>
</tr>

<tr>
	<td>2. Accept-Language</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Прибавляет к имени файла расширение и<br>
		ищет их в порядке перечисления.
	</td>
</tr>

<tr>
	<td>3. Allow</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Выводит разрещенные методы, если<br>
		использовали неразрешенные(или метод OPTIONS)
	</td>
</tr>

<tr>
	<td>4. Authorization</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Добавил auth_basic и auth_basic_user_file,<br>
		как в nginx, сверяет данные ввода с теми, что в<br>
		файле auth_basic_user_file
	</td>
</tr>

<tr>
	<td>5. Content-Language</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Если находим файл на подходящем языке,<br>
		то запоминаем язык и потом используем.
	</td>
</tr>

<tr>
	<td>6. Content-Length</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>Размер тела сообщения клиента.</td>
</tr>

<tr>
	<td>7. Content-Location</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>Относительное расположение файла</td>
</tr>

<tr>
	<td>8. Content-Type</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>Тип отправляемых сервером данных.</td>
</tr>

<tr>
	<td>9. Date</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>Дата составления ответа.</td>
</tr>

<tr>
	<td>10. Host</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>Цель обращения клиента.</td>
</tr>

<tr>
	<td>11. Last-Modified</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>Время последнего изменения файла.</td>
</tr>

<tr>
	<td>12. Location</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>URI локации запроса</td>
</tr>

<tr>
	<td>13. Referer</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Содержит информацию, к какому<br>
		запросу относится текущий.
	</td>
</tr>

<tr>
	<td>14. Retry-After</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Сколько клиент должен ожидать<br>
		перед запросом.
	</td>
</tr>

<tr>
	<td>15. Server</td>
	<td style="font-weight: bold">Выполнено</td>
	<td></td>
</tr>

<tr>
	<td>16. Transfer-Encoding</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		chunked - означает, что размер<br>
		тела неизвестен.
	</td>
</tr>

<tr>
	<td>17. User-Agent</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>Содержит информацию о клиенте.</td>
</tr>


<tr>
	<td>18. WWW-Authenticate</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		В случае, если в конфигурации указано<br>
		auth_basic "Text", то данный заголовок<br>
		посылается ответ и ожидает ввода логина и пароля<br>
	</td>
</tr>

</table>

<p style="font: 30px Verdana sans-serif;" align="center">
Парсер
</p>

<table style="font: 20px Verdana sans-serif; text-align: center;">

<tr> 
	<td>Функционал</td>
	<td>Статус</td>
	<td>Комментарий</td>
</tr>

<tr>
	<td>1. Port, host</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Можно указывать только один порт listen.<br>
		host в формате IP-адреса.
	</td>
</tr>

<tr>
	<td>2. Server names</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Можно указывать несколько имен сервера,<br>
		ищется сервер по заголовку host, с таким<br>
		же портом и именем или портом.
	</td>
</tr>

<tr>
	<td>3. Default pages</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Формат:<br>
		error_page номер_ошибки путь
	</td>
</tr>

<tr>
	<td>4. Limit client body size</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Формат:<br>
		client_max_body_size число
	</td>
</tr>

<tr>
	<td>5. Accepted HTTP methods</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Формат:<br>
		allowed_method *(delim метод)
	</td>
</tr>

<tr>
	<td>6. Root</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Формат:<br>
		root путь
	</td>
</tr>

<tr>
	<td>7. Autoindex</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Формат:<br>
		autoindex on/off<br>
		* Работает в случае, если нет индексных файлов
	</td>
</tr>

<tr>
	<td>8. Index</td>
	<td style="font-weight: bold">Выполнено</td>
	<td>
		Формат:<br>
		index *(delim файл)
	</td>
</tr>

<tr>
	<td>9. CGI(Chunked request)</td>
	<td>В процессе</td>
	<td></td>
</tr>

</table>

