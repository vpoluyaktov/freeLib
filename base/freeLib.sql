/* Disable foreign keys */
PRAGMA foreign_keys = 'off';

/* Begin transaction */
BEGIN;

/* Database properties */
PRAGMA auto_vacuum = 0;
PRAGMA encoding = 'UTF-8';
PRAGMA page_size = 32768;

/* Drop table [lib] */
DROP TABLE IF EXISTS [main].[lib];

/* Table structure [lib] */
CREATE TABLE [main].[lib](
  [id] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, 
  [name] TEXT, 
  [path] TEXT, 
  [inpx] TEXT, 
  [firstAuthor] BOOL, 
  [woDeleted] BOOL, 
  [currentTab] INTEGER, 
  [currentAuthor] INTEGER, 
  [currentSeria] INTEGER, 
  [currentGenre] INTEGER, 
  [currentGroup] INTEGER, 
  [currentBookForAuthor] INTEGER, 
  [currentBookForSeria] INTEGER, 
  [currentBookForGenre] INTEGER, 
  [currentBookForGroup] INTEGER, 
  [currentSearchingFilter] TEXT, 
  [currentTag] INTEGER, 
  [currentBookLanguage] TEXT NOT NULL DEFAULT '*');

/* Drop table [tag] */
DROP TABLE IF EXISTS [main].[tag];

/* Table structure [tag] */
CREATE TABLE [main].[tag](
  [id] INTEGER PRIMARY KEY NOT NULL, 
  [color] TEXT DEFAULT (NULL), 
  [name] TEXT);

/* Drop table [author] */
DROP TABLE IF EXISTS [main].[author];

/* Table structure [author] */
CREATE TABLE [main].[author](
  [id] INTEGER PRIMARY KEY, 
  [LastName] TEXT, 
  [FirstName] TEXT, 
  [MiddleName] TEXT, 
  [NickName] TEXT, 
  [id_lib] INTEGER REFERENCES [lib]([id]), 
  [tag] INTEGER REFERENCES [tag]([id]));
CREATE INDEX [main].[author_sort]
ON [author](
  [LastName] ASC, 
  [FirstName] ASC, 
  [MiddleName] ASC);

/* Drop table [seria] */
DROP TABLE IF EXISTS [main].[seria];

/* Table structure [seria] */
CREATE TABLE [main].[seria](
  [id] INTEGER PRIMARY KEY, 
  [name] TEXT, 
  [id_lib] INTEGER REFERENCES [lib]([id]), 
  [tag] INTEGER REFERENCES [tag]([id]));
CREATE INDEX [main].[seria_name]
ON [seria](
  [name] ASC, 
  [id_lib] ASC);
CREATE INDEX [main].[seria_id] ON [seria]([id] ASC);

/* Drop table [book] */
DROP TABLE IF EXISTS [main].[book];

/* Table structure [book] */
CREATE TABLE [main].[book](
  [id] INTEGER PRIMARY KEY, 
  [name] TEXT, 
  [star] INTEGER DEFAULT 0, 
  [id_seria] INTEGER REFERENCES [seria]([id]), 
  [num_in_seria] INTEGER DEFAULT 0, 
  [language] TEXT, 
  [id_lib] INTEGER REFERENCES [lib]([id]), 
  [file] TEXT, 
  [size] INTEGER, 
  [deleted] BOOL DEFAULT False, 
  [date] DATETIME, 
  [format] TEXT, 
  [keys] TEXT, 
  [id_inlib] INTEGER, 
  [archive] TEXT, 
  [first_author_id] INTEGER, 
  [tag] INTEGER REFERENCES [tag]([id]), 
  [readed] BOOL DEFAULT False);
CREATE INDEX [main].[book_seria] ON [book]([id_seria] ASC);
CREATE INDEX [main].[book_file]
ON [book](
  [id_lib], 
  [file], 
  [archive]);
CREATE INDEX [main].[book_first_author] ON [book]([first_author_id] ASC);

/* Drop table [book_author] */
DROP TABLE IF EXISTS [main].[book_author];

/* Table structure [book_author] */
CREATE TABLE [main].[book_author](
  [id_book] INTEGER REFERENCES [book]([id]), 
  [id_author] INTEGER REFERENCES [author]([id]), 
  [id_lib] INTEGER REFERENCES [lib]([id]), 
  [language] TEXT, 
  PRIMARY KEY([id_book], [id_author], [id_lib]));
CREATE INDEX [main].[book_author_id_book] ON [book_author]([id_book] ASC);
CREATE INDEX [main].[book_author_id_author] ON [book_author]([id_author]);

/* Drop table [book_genre] */
DROP TABLE IF EXISTS [main].[book_genre];

/* Table structure [book_genre] */
CREATE TABLE [main].[book_genre](
  [id_book] INTEGER, 
  [id_genre] INTEGER, 
  [id_lib] INTEGER REFERENCES [lib]([id]), 
  [language] TEXT, 
  PRIMARY KEY([id_book], [id_genre], [id_lib]));
CREATE INDEX [main].[book_genre_id] ON [book_genre]([id_genre] ASC);
CREATE INDEX [main].[book_genre_id_book] ON [book_genre]([id_book] ASC);

/* Drop table [groups] */
DROP TABLE IF EXISTS [main].[groups];

/* Table structure [groups] */
CREATE TABLE [main].[groups](
  [id] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, 
  [name] TEXT NOT NULL, 
  [id_lib] INTEGER REFERENCES [lib]([id]), 
  [blocked] BOOL DEFAULT False, 
  [blocked_name] TEXT, 
  [icon] BLOB);
CREATE INDEX [main].[group_name] ON [groups]([name] ASC);

/* Drop table [book_group] */
DROP TABLE IF EXISTS [main].[book_group];

/* Table structure [book_group] */
CREATE TABLE [main].[book_group](
  [book_id] INTEGER NOT NULL CONSTRAINT [fk_book_group_to_books] REFERENCES [book]([id]) ON DELETE CASCADE ON UPDATE CASCADE, 
  [group_id] INTEGER NOT NULL CONSTRAINT [fk_book_group_to_groups] REFERENCES [groups]([id]) ON DELETE CASCADE ON UPDATE CASCADE, 
  [id_lib] INTEGER NOT NULL CONSTRAINT [fk_book_group_to_lib] REFERENCES [lib]([id]) ON DELETE CASCADE ON UPDATE CASCADE, 
  PRIMARY KEY([book_id], [group_id], [id_lib]));

/* Drop table [genre] */
DROP TABLE IF EXISTS [main].[genre];

/* Table structure [genre] */
CREATE TABLE [main].[genre](
  [id] INTEGER PRIMARY KEY, 
  [name] TEXT, 
  [name_en] TEXT, 
  [id_parent] INTEGER, 
  [sort_index] INTEGER, 
  [code] TEXT DEFAULT (NULL));

/* Drop table [objects_without_data] */
DROP TABLE IF EXISTS [main].[objects_without_data];

/* Table structure [objects_without_data] */
CREATE TABLE [main].[objects_without_data](
  [id] INTEGER PRIMARY KEY, 
  [id_lib] INTEGER, 
  [id_book_without_title] INTEGER, 
  [id_author_without_data] INTEGER, 
  [id_seria_without_name] INTEGER, 
  [id_genre_without_name] INTEGER);

/* Drop table [params] */
DROP TABLE IF EXISTS [main].[params];

/* Table structure [params] */
CREATE TABLE [main].[params](
  [id] INTEGER PRIMARY KEY, 
  [name] TEXT NOT NULL UNIQUE, 
  [value] TEXT);

/* Table data [lib] Record count: 0 */

/* Table data [tag] Record count: 7 */
INSERT INTO [tag]([rowid], [id], [color], [name]) VALUES(1, 1, '#ff888b', 'Красный');
INSERT INTO [tag]([rowid], [id], [color], [name]) VALUES(2, 2, '#fec84f', 'Оранжевый');
INSERT INTO [tag]([rowid], [id], [color], [name]) VALUES(3, 3, '#fdf14b', 'Желтый');
INSERT INTO [tag]([rowid], [id], [color], [name]) VALUES(4, 4, '#baf144', 'Зеленый');
INSERT INTO [tag]([rowid], [id], [color], [name]) VALUES(5, 5, '#97d2fd', 'Синий');
INSERT INTO [tag]([rowid], [id], [color], [name]) VALUES(6, 6, '#edb2fd', 'Лиловый');
INSERT INTO [tag]([rowid], [id], [color], [name]) VALUES(7, 7, '#c8c8c8', 'Серый');

/* Table data [author] Record count: 0 */

/* Table data [seria] Record count: 0 */

/* Table data [book] Record count: 0 */

/* Table data [book_author] Record count: 0 */

/* Table data [book_genre] Record count: 0 */

/* Table data [groups] Record count: 0 */

/* Table data [book_group] Record count: 0 */

/* Table data [genre] Record count: 505 */
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(1, 1, 'Военное дело', 'Military', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(2, 2, 'Боевые искусства', 'Military Arts', 1, 10, 'military_arts');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(3, 3, 'Военная документалистика и аналитика', 'Military Documentaries And Analytics', 1, 20, 'nonf_military');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(4, 4, 'Военная история', 'Military History', 1, 30, 'military_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(5, 5, 'Военная техника и вооружение', 'Weapon', 1, 40, 'military_weapon');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(6, 6, 'Военное дело: прочее', 'Military Other', 1, 50, 'military');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(7, 7, 'Военные мемуары', 'Military Memoirs', 1, 60, 'military_memoirs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(8, 8, 'Спецслужбы', 'Military Special', 1, 70, 'military_special');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(9, 9, 'Деловая литература', 'Economy, Business', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(10, 10, 'Банковское дело', 'Banking', 9, 10, 'banking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(11, 11, 'Бухучет и аудит', 'Accounting', 9, 20, 'accounting');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(12, 12, 'Внешняя торговля', 'Global Economy', 9, 30, 'global_economy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(13, 13, 'Деловая литература: прочее', 'Business Literature', 9, 40, 'sci_business');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(14, 14, 'Делопроизводство', 'Paper Work', 9, 50, 'paper_work');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(15, 15, 'Корпоративная культура', 'Corporate Culture', 9, 60, 'org_behavior');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(16, 16, 'Личные финансы', 'Personal Finance', 9, 70, 'personal_finance');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(17, 17, 'Малый бизнес', 'Small Business', 9, 80, 'small_business');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(18, 18, 'Маркетинг, PR, реклама', 'Marketing, PR, Adv', 9, 90, 'marketing');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(19, 19, 'Недвижимость', 'Real Estate', 9, 100, 'real_estate');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(20, 20, 'О бизнесе популярно', 'Popular Business', 9, 110, 'popular_business');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(21, 21, 'Отраслевые издания', 'Industries', 9, 120, 'industries');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(22, 22, 'Поиск работы, карьера', 'Job Hunting', 9, 130, 'job_hunting');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(23, 23, 'Практическая экономика', 'Economics', 9, 140, 'economics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(24, 24, 'Справочная деловая литература', 'economics_ref', 9, 150, 'economics_ref');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(25, 25, 'Торговля', 'Trade', 9, 160, 'trade');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(26, 26, 'Управление, подбор персонала', 'Management', 9, 170, 'management');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(27, 27, 'Ценные бумаги, инвестиции', 'Stock', 9, 180, 'stock');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(28, 28, 'Детективы и триллеры', 'Detectives & Thrillers', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(29, 29, 'Археологический триллер', 'Archaeological Thriller', 28, 10, 'thriller_archaeological');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(30, 30, 'Боевик', 'Action', 28, 20, 'det_action');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(31, 31, 'Дамский детектив', 'Cozy Mysteries', 28, 30, 'det_cozy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(32, 32, 'Детектив', 'Detectives', 28, 40, 'detective');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(33, 33, 'Зарубежный детектив', 'Foreign Detectives', 28, 50, 'foreign_detective');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(34, 34, 'Иронический детектив', 'Ironical Detectives', 28, 60, 'det_irony');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(35, 35, 'Исторический детектив', 'Historical Detectives', 28, 70, 'det_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(36, 36, 'Классический детектив', 'Classical Detectives', 28, 80, 'det_classic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(37, 37, 'Криминальный детектив', 'Crime Detectives', 28, 90, 'det_crime');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(38, 38, 'Крутой детектив', 'Hard-boiled', 28, 100, 'det_hard');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(39, 39, 'Маньяки', 'Maniacs', 28, 110, 'det_maniac');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(40, 40, 'Медицинский триллер', 'Medical Thriller', 28, 120, 'thriller_medical');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(41, 41, 'Политический детектив', 'Political Detectives', 28, 130, 'det_political');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(42, 42, 'Полицейский детектив', 'Police Stories', 28, 140, 'det_police');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(43, 43, 'Российский детектив', 'Russian Detectives', 28, 150, 'det_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(44, 44, 'Советский детектив', 'Soviet Detectives', 28, 160, 'det_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(45, 45, 'Техно триллер', 'Techno Thriller', 28, 170, 'thriller_techno');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(46, 46, 'Триллер', 'Thrillers', 28, 180, 'thriller');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(47, 47, 'Шпионский детектив', 'Espionage Detectives', 28, 190, 'det_espionage');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(48, 48, 'Юридический триллер', 'Legal Thriller', 28, 200, 'thriller_legal');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(49, 49, 'Детская литература', 'Childrens', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(50, 50, 'Детская литература: прочее', 'Childrens Other', 49, 10, 'children');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(51, 51, 'Детская образовательная литература', 'Childrens Educational', 49, 20, 'child_education');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(52, 52, 'Детская остросюжетная литература', 'Childrens Detectives & Thrillers', 49, 30, 'child_det');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(53, 53, 'Детская проза', 'Childrens Prose', 49, 40, 'child_prose');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(54, 54, 'Детские приключения', 'Childrens Adventures', 49, 50, 'child_adv');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(55, 55, 'Детские стихи', 'Childrens Verses', 49, 60, 'child_verse');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(56, 56, 'Детские ужастики', 'Childrens Horror', 49, 70, 'child_horror');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(57, 57, 'Детская фантастика', 'Childrens Science Fiction', 49, 80, 'child_sf');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(58, 58, 'Детский фольклор', 'Child Folklore', 49, 90, 'child_folklore');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(59, 59, 'Детское фэнтези', 'Childrens Fantasy', 49, 100, 'child_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(60, 60, 'Для самых маленьких', 'For Babies', 49, 110, 'child_baby');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(61, 61, 'Зарубежная детская литература', 'Foreign Literature For Children', 49, 120, 'foreign_children');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(62, 62, 'Классическая детская литература', 'Classical Childrens Literature', 49, 130, 'child_classical');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(63, 63, 'Подростковая литература', 'Young-adult Fiction', 49, 140, 'ya');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(64, 64, 'Русская детская литература', 'Russian Childrens Literature', 49, 150, 'child_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(65, 65, 'Русские сказки для детей', 'Russian Fairy Tales', 49, 160, 'child_tale_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(66, 66, 'Сказки для детей', 'Fairy Tales', 49, 170, 'child_tale');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(67, 67, 'Советская детская литература', 'Soviet Childrens Literature', 49, 180, 'child_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(68, 68, 'Документальная литература', 'Nonfiction', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(69, 69, 'Беседы и интервью', 'Interview', 68, 10, 'interview');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(70, 70, 'Биографии и мемуары', 'Biography & Memoirs', 68, 20, 'nonf_biography');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(71, 71, 'Документальная литература', 'Nonfiction Other', 68, 30, 'nonfiction');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(72, 72, 'Заметки натуралиста', 'Naturalist Notes', 68, 40, 'naturalist_notes');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(73, 73, 'Записки путешественника', 'Travel Notes', 68, 50, 'travel_notes');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(74, 74, 'Письма, переписки', 'Letters & Correspondence', 68, 60, 'letters');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(75, 75, 'Публицистика', 'Publicism', 68, 70, 'nonf_publicism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(76, 76, 'Речи, выступления, доклады', 'Speech', 68, 80, 'speech');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(77, 77, 'Домоводство (Дом и семья)', 'Home & Family', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(78, 78, 'Выживание и личная безопасность', 'Survival', 77, 10, 'home_survival');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(79, 79, 'Дети. Книги для родителей', 'Child', 77, 20, 'home_child');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(80, 80, 'Дом и семья: прочее', 'Home Other', 77, 30, 'home');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(81, 81, 'Домашние животные', 'Pets', 77, 40, 'home_pets');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(82, 82, 'Домоводство', 'Housekeeping', 77, 50, 'home_housekeeping');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(83, 83, 'Здоровье', 'Health', 77, 60, 'home_health');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(84, 84, 'Кошки', 'Cats', 77, 70, 'cats');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(85, 85, 'Красота', 'Beauty', 77, 80, 'home_beauty');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(86, 86, 'Кулинария', 'Cooking', 77, 90, 'home_cooking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(87, 87, 'Развлечения', 'Entertaining', 77, 100, 'home_entertain');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(88, 88, 'Семейные отношения', 'Family', 77, 110, 'family');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(89, 89, 'Собаки', 'Dogs', 77, 120, 'dogs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(90, 90, 'Эротика и секс', 'Erotica & Sex', 77, 130, 'home_sex');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(91, 91, 'Драматургия', 'Dramaturgy', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(92, 92, 'Античная драма', 'Antique Drama', 91, 10, 'drama_antique');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(93, 93, 'Водевиль, буффонада', 'Vaudeville', 91, 20, 'vaudeville');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(94, 94, 'Драма', 'Drama', 91, 30, 'drama');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(95, 95, 'Драматургия', 'Dramaturgy', 91, 40, 'dramaturgy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(96, 96, 'Зарубежная классическая драматургия', 'Foreign Dramaturgy', 91, 50, 'dramaturgy_for_classic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(97, 97, 'Зарубежная современная драматургия', 'Contemporary Foreign Dramaturgy', 91, 60, 'foreign_dramaturgy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(98, 98, 'Киносценарий', 'Screenplays', 91, 70, 'screenplays');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(99, 99, 'Комедия', 'Comedy', 91, 80, 'comedy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(100, 100, 'Мистерия', 'Mystery', 91, 90, 'mystery');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(101, 101, 'Русская классическая драматургия', 'Russian Dramaturgy', 91, 100, 'dramaturgy_rus_classic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(102, 102, 'Русская современная драматургия', 'Contemporary Russian Dramaturgy', 91, 110, 'dramaturgy_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(103, 103, 'Советская драматургия', 'Soviet Dramaturgy', 91, 120, 'dramaturgy_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(104, 104, 'Сценарий', 'Scenarios', 91, 130, 'scenarios');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(105, 105, 'Трагедия', 'Tragedy', 91, 140, 'tragedy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(106, 106, 'История', 'History', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(107, 107, 'История России', 'History of Russia', 106, 10, 'history_russia');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(108, 108, 'История Европы', 'History of Europe', 106, 20, 'history_europe');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(109, 109, 'История Азии', 'History of Asia', 106, 30, 'history_asia');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(110, 110, 'История Африки', 'History of Africa', 106, 40, 'history_africa');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(111, 111, 'История Америки', 'History of America', 106, 50, 'history_america');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(112, 112, 'История Австралии и Океании', 'History of Australia and Oceania', 106, 60, 'history_australia');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(113, 113, 'Мировая история', 'History of World', 106, 70, 'history_world');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(114, 114, 'История первобытного общества', 'History of Primitive Society', 106, 80, 'sci_history_0');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(115, 115, 'История Древнего мира', 'Ancient World History', 106, 90, 'sci_history_4');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(116, 116, 'История Средних веков', 'Medieval History', 106, 100, 'sci_history_15');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(117, 117, 'История Нового времени', 'History of Modern Times', 106, 110, 'sci_history_18');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(118, 118, 'Новейшая история', 'Newest History', 106, 120, 'sci_history_20');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(119, 119, 'Современная история', 'Contemporary History', 106, 130, 'sci_history_21');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(120, 120, 'История: прочее', 'History Other', 106, 140, 'sci_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(121, 121, 'Компьютеры и Интернет', 'Computers & Internet', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(122, 122, 'Web-дизайн', 'Web Design', 121, 10, 'comp_www_design');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(123, 123, 'Базы данных', 'Databases', 121, 20, 'comp_db');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(124, 124, 'Графика. Дизайн. Мультимедиа', 'Design', 121, 30, 'comp_design');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(125, 125, 'Интернет', 'Internet', 121, 40, 'comp_www');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(126, 126, 'История информатики и вычислительной техники', 'Computers History', 121, 50, 'comp_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(127, 127, 'Компьютерная безопасность', 'Security', 121, 60, 'comp_security');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(128, 128, 'Компьютерное железо', 'Hardware', 121, 70, 'comp_hard');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(129, 129, 'Околокомпьютерная литература', 'Computers Other', 121, 80, 'computers');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(130, 130, 'Офисные приложения', 'Office Software', 121, 90, 'comp_soft_office');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(131, 131, 'Программы', 'Software', 121, 100, 'comp_soft');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(132, 132, 'САПР', 'CAD, CAM and CAE', 121, 110, 'comp_soft_cad');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(133, 133, 'Сети', 'Networking', 121, 120, 'comp_osnet');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(134, 134, 'Учебники и самоучители по компьютеру', 'Computers Textbook', 121, 130, 'tbg_computers');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(135, 135, 'Хакерство', 'Hacking', 121, 140, 'comp_hacking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(136, 136, 'Цифровая обработка сигналов', 'Digital Signal Processing', 121, 150, 'comp_dsp');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(137, 137, 'Компьютеры: Операционные системы', 'OS', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(138, 138, 'Android', 'Android', 137, 10, 'comp_os_android');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(139, 139, 'FreeDOS', 'FreeDOS', 137, 20, 'comp_os_freedos');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(140, 140, 'Linux', 'Linux', 137, 30, 'comp_os_linux');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(141, 141, 'MacOS', 'MacOS', 137, 40, 'comp_os_macos');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(142, 142, 'MS-DOS', 'MS-DOS', 137, 50, 'comp_os_msdos');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(143, 143, 'OS/2', 'OS/2', 137, 60, 'comp_os_os2');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(144, 144, 'Unix', 'Unix', 137, 70, 'comp_os_unix');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(145, 145, 'Windows', 'Windows', 137, 80, 'comp_os_windows');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(146, 146, 'ОС: администрирование, мониторинг, диагностика', 'OS Administration', 137, 90, 'comp_os_admin');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(147, 147, 'ОС: теоретические вопросы', 'OS Theory', 137, 100, 'comp_os_theory');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(148, 148, 'Прочие ОС', 'Other OS', 137, 110, 'comp_os');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(149, 149, 'Компьютеры: Разработка ПО', 'Soft Development', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(150, 150, 'Алгоритмы и структуры данных', 'Algorithms', 149, 10, 'comp_soft_dev_alg');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(151, 151, 'Базы знаний и экспертные системы', 'Expert Systems', 149, 20, 'comp_db_exp');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(152, 152, 'Искусственный интеллект', 'Artifical Intellegence', 149, 30, 'comp_dv_ai');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(153, 153, 'Крэкинг и реверсинжиниринг', 'Craking And Reverse Engineering', 149, 40, 'comp_soft_dev_craking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(154, 154, 'Менеджмент ПО', 'Management', 149, 50, 'comp_soft_dev_man');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(155, 155, 'Объектно-ориентированное программирование', 'OOP', 149, 60, 'comp_soft_dev_oop');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(156, 156, 'Отладка, тестирование и оптимизация ПО', 'Debuging', 149, 70, 'comp_soft_dev_debug');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(157, 157, 'Параллельное и распределенное программирование', 'Parallel Programming', 149, 80, 'comp_soft_dev_parallel');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(158, 158, 'Программирование графики', 'Graphic Programming', 149, 90, 'comp_soft_dev_graphic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(159, 159, 'Программирование игр', 'Games Programming', 149, 100, 'comp_soft_dev_games');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(160, 160, 'Программирование: прочее', 'Programming Other', 149, 110, 'comp_soft_dev');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(161, 161, 'Системное программирование', 'System Programming', 149, 120, 'comp_soft_dev_system');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(162, 162, 'Компьютеры: Языки и системы программирования', 'Programming Languages', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(163, 163, '.NET Framework', '.NET Framework', 162, 10, 'comp_prog_dotnet');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(164, 164, 'Ada', 'Ada', 162, 20, 'comp_prog_ada');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(165, 165, 'Assembler', 'Assembler', 162, 30, 'comp_prog_assembler');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(166, 166, 'Basic, Visual Basic, VB Script, VBA и т.п.', 'Basic, Visual Basic, VB Script, VBA', 162, 40, 'comp_prog_basic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(167, 167, 'C, C++, C#', 'C, C++, C#', 162, 50, 'comp_prog_c');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(168, 168, 'Forth', 'Forth', 162, 60, 'comp_prog_forth');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(169, 169, 'Fortran', 'Fortran', 162, 70, 'comp_prog_fortran');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(170, 170, 'Java, Java Script', 'Java, Java Script', 162, 80, 'comp_prog_java');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(171, 171, 'Lisp, Scheme', 'Lisp, Scheme', 162, 90, 'comp_prog_lisp');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(172, 172, 'Lua', 'Lua', 162, 100, 'comp_prog_lua');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(173, 173, 'MFC', 'MFC', 162, 110, 'comp_prog_mfc');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(174, 174, 'Modula-2, Modula-3, Oberon, Oberon-2, Component Pascal', 'Modula-2, Modula-3, Oberon, Oberon-2, Component Pascal', 162, 120, 'comp_prog_oberon');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(175, 175, 'Pascal, Delphi, Lazarus и т.п.', 'Pascal, Delphi, Lazarus', 162, 130, 'comp_prog_pascal');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(176, 176, 'PHP', 'PHP', 162, 140, 'comp_prog_php');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(177, 177, 'Prolog', 'Prolog', 162, 150, 'comp_prog_prolog');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(178, 178, 'Python', 'Python', 162, 160, 'comp_prog_python');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(179, 179, 'Qt', 'Qt', 162, 170, 'comp_prog_qt');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(180, 180, 'Ruby', 'Ruby', 162, 180, 'comp_prog_ror');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(181, 181, 'Windows API', 'Windows API', 162, 190, 'comp_prog_winapi');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(182, 182, 'Другие языки и системы программирования', 'Other Programming Languages', 162, 200, 'comp_programming');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(183, 183, 'Культура и искусство', 'Art, Art History, Design', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(184, 184, 'Архитектура и скульптура', 'Architecture', 183, 10, 'architecture_book');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(185, 185, 'Живопись, альбомы, иллюстрированные каталоги', 'Painting', 183, 20, 'painting');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(186, 186, 'Изобразительное искусство, фотография', 'Visual Arts', 183, 30, 'visual_arts');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(187, 187, 'Искусство и дизайн', 'Art, Design', 183, 40, 'design');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(188, 188, 'Искусствоведение', 'Art Criticism', 183, 50, 'art_criticism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(189, 189, 'История искусства', 'Art History', 183, 60, 'art_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(190, 190, 'Кино', 'Cine', 183, 70, 'cine');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(191, 191, 'Критика', 'Criticism', 183, 80, 'nonf_criticism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(192, 192, 'Культурология', 'Cultural Science', 183, 90, 'sci_culture');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(193, 193, 'Мировая художественная культура', 'Art World Culture', 183, 100, 'art_world_culture');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(194, 194, 'Мода и стиль', 'Fashion Style', 183, 110, 'fashion_style');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(195, 195, 'Музыка', 'Music', 183, 120, 'music');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(196, 196, 'Радио и телевидение', 'Radio and TV', 183, 130, 'radio_tv');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(197, 197, 'Танцы и хореография', 'Dance and Choreography', 183, 140, 'art_dance');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(198, 198, 'Театр', 'Theatre', 183, 150, 'theatre');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(199, 199, 'Литература по изданиям', 'Publications', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(200, 200, 'Дореволюционные зарубежные издания', 'Pre-revolutionary Foreign Publications', 199, 10, 'old_foreign_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(201, 201, 'Дореволюционные российские издания', 'Pre-revolutionary Russian Publications', 199, 20, 'old_rus_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(202, 202, 'Зарубежные издания советского периода', 'Foreign publications of the Soviet period', 199, 30, 'foreign_su_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(203, 203, 'Раритетные издания', 'Rare Editions', 199, 40, 'rarity');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(204, 204, 'Самиздат, сетевая литература', 'Network Literature', 199, 50, 'network_literature');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(205, 205, 'Советские издания', 'Soviet Publications', 199, 60, 'su_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(206, 206, 'Современные зарубежные издания', 'Modern Foreign Publications', 199, 70, 'foreign_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(207, 207, 'Современные издания стран бывшего СССР', 'Publications Of The Former USSR', 199, 80, 'ex_su_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(208, 208, 'Современные российские издания', 'Modern Russian Publications', 199, 90, 'rus_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(209, 209, 'Литература по эпохам', 'Literature Ages', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(210, 210, 'Литература IV века и ранее (эпоха Древнего мира)', 'Literature Before The V Century', 209, 10, 'literature_4');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(211, 211, 'Литература V-XIII веков (эпоха Средневековья)', 'Literature Of V-XIII Centuries', 209, 20, 'foreign_antique');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(212, 212, 'Литература XIV-XVI веков (эпоха Возрождения)', 'Literature Of XIV-XVI Centuries', 209, 30, 'literature_16');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(213, 213, 'Литература XVII-XVIII веков (эпоха Просвящения)', 'Literature Of XVII-XVIII Centuries', 209, 40, 'literature_18');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(214, 214, 'Литература ХIX века (эпоха Промышленной революции)', 'Literature Of The XIX Century', 209, 50, 'literature_19');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(215, 215, 'Литература ХX века (эпоха Социальных революций)', 'Literature Of The ХX Century', 209, 60, 'literature_20');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(216, 216, 'Литература ХXI века (эпоха Глобализации экономики)', 'Literature Of The ХXI Century', 209, 70, 'literature_21');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(217, 217, 'Любовные романы', 'Romance', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(218, 218, 'Зарубежная литература о любви', 'Foreign Love Romance', 217, 10, 'foreign_love');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(219, 219, 'Исторические любовные романы', 'Historical Romance', 217, 20, 'love_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(220, 220, 'Короткие любовные романы', 'Short Romance', 217, 30, 'love_short');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(221, 221, 'Любовная фантастика', 'Love SF', 217, 40, 'love_sf');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(222, 222, 'Любовное фэнтези', 'Love Fantasy', 217, 50, 'love_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(223, 223, 'Любовные детективы', 'Detective Romance', 217, 60, 'love_detective');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(224, 224, 'О любви', 'Love Romance', 217, 70, 'love');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(225, 225, 'Порно', 'Porno', 217, 80, 'love_hard');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(226, 226, 'Русская литература о любви', 'Russian Love Romance', 217, 90, 'love_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(227, 227, 'Сексуальные извращения', 'Sexual Perversion', 217, 100, 'sexual_perversion');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(228, 228, 'Слэш', 'Love Slash', 217, 110, 'love_slash');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(229, 229, 'Советская литература о любви', 'Soviet Love Romance', 217, 120, 'love_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(230, 230, 'Современные любовные романы', 'Contemporary Romance', 217, 130, 'love_contemporary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(231, 231, 'Фемслеш', 'Love Femslash', 217, 140, 'love_femslash');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(232, 232, 'Эротика', 'Erotica', 217, 150, 'love_erotica');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(233, 233, 'Наука и образование: прочее', 'Scientific-educational', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(234, 234, 'Альтернативная медицина', 'Alternative Medicine', 233, 10, 'sci_medicine_alternative');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(235, 235, 'История науки', 'Science History', 233, 20, 'science_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(236, 236, 'Научная литература', 'Science Other', 233, 30, 'science');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(237, 237, 'Научпоп', 'Science Popular', 233, 40, 'sci_popular');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(238, 238, 'Паранаука, псевдонаука, альтернативные теории', 'Theories', 233, 50, 'sci_theories');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(239, 239, 'Науки естественные', 'Natural Sciences', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(240, 240, 'Аналитическая химия', 'Analitic Chemistry', 239, 10, 'sci_anachem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(241, 241, 'Астрономия и Космос', 'Cosmos', 239, 20, 'sci_cosmos');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(242, 242, 'Ветеринария', 'Veterinary', 239, 30, 'sci_veterinary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(243, 243, 'География', 'Geography', 239, 40, 'sci_geography');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(244, 244, 'Геологические науки и горное дело', 'Geology and Geophysics', 239, 50, 'sci_geo');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(245, 245, 'Математика', 'Mathematics', 239, 60, 'sci_math');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(246, 246, 'Медицина', 'Medicine', 239, 70, 'sci_medicine');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(247, 247, 'Органическая химия', 'Органическая химия', 239, 80, 'sci_orgchem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(248, 248, 'Физика', 'Physics', 239, 90, 'sci_phys');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(249, 249, 'Физическая химия', 'Physical Chemistry', 239, 100, 'sci_physchem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(250, 250, 'Химия', 'Chemistry', 239, 110, 'sci_chem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(251, 251, 'Науки о живой природе', 'Biology', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(252, 252, 'Биология', 'Biology', 251, 10, 'sci_biology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(253, 253, 'Биофизика', 'Biophysics', 251, 20, 'sci_biophys');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(254, 254, 'Биохимия', 'Biochemistry', 251, 30, 'sci_biochem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(255, 255, 'Ботаника', 'Botany', 251, 40, 'sci_botany');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(256, 256, 'Зоология', 'Zoology', 251, 50, 'sci_zoo');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(257, 257, 'Палеонтология', 'Paleontology', 251, 60, 'sci_paleontology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(258, 258, 'Эволюционизм', 'Evolutionism', 251, 70, 'sci_evolutionism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(259, 259, 'Экология и защита природы', 'Ecology', 251, 80, 'sci_ecology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(260, 260, 'Науки обществ. и гуманитарные', 'Humanities', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(261, 261, 'Археология', 'Archeology', 260, 10, 'sci_archeology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(262, 262, 'Востоковедение', 'Science Oriental', 260, 20, 'sci_oriental');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(263, 263, 'Государство и право', 'State Science', 260, 30, 'sci_state');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(264, 264, 'Иностранные языки', 'Foreign Languages', 260, 40, 'foreign_language');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(265, 265, 'Краеведение', 'Study of Local Lore', 260, 50, 'local_lore_study');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(266, 266, 'Литературоведение', 'Philology', 260, 60, 'sci_philology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(267, 267, 'Педагогика', 'Pedagogy', 260, 70, 'sci_pedagogy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(268, 268, 'Политика и дипломатия', 'Politics and Diplomacy', 260, 80, 'sci_politics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(269, 269, 'Социология', 'Social Studies', 260, 90, 'sci_social_studies');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(270, 270, 'Философия', 'Philosophy', 260, 100, 'sci_philosophy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(271, 271, 'Экономика', 'Economic', 260, 110, 'sci_economy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(272, 272, 'Юриспруденция', 'Jurisprudence', 260, 120, 'sci_juris');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(273, 273, 'Языкознание', 'Linguistics', 260, 130, 'sci_linguistic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(274, 274, 'Поэзия', 'Poetry', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(275, 275, 'Басни', 'Fable', 274, 10, 'fable');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(276, 276, 'В стихах', 'In Verse', 274, 20, 'in_verse');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(277, 277, 'Верлибры', 'Vers Libre', 274, 30, 'vers_libre');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(278, 278, 'Визуальная поэзия', 'Visual Poetry', 274, 40, 'visual_poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(279, 279, 'Классическая зарубежная поэзия', 'Classical Foreign Poetry', 274, 50, 'poetry_for_classical');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(280, 280, 'Классическая поэзия', 'Classical Poetry', 274, 60, 'poetry_classical');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(281, 281, 'Классическая русская поэзия', 'Classical Russian Poetry', 274, 70, 'poetry_rus_classical');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(282, 282, 'Лирика', 'Lyrics', 274, 80, 'lyrics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(283, 283, 'Палиндромы', 'Palindromes', 274, 90, 'palindromes');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(284, 284, 'Песенная поэзия', 'Song Poetry', 274, 100, 'song_poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(285, 285, 'Поэзия', 'Poetry', 274, 110, 'poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(286, 286, 'Поэзия Востока', 'Poetry Of The East', 274, 120, 'poetry_east');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(287, 287, 'Поэма', 'Poem', 274, 130, 'poem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(288, 288, 'Советская поэзия', 'Soviet Poetry', 274, 140, 'poetry_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(289, 289, 'Современная зарубежная поэзия', 'Modern Foreign Poetry', 274, 150, 'poetry_for_modern');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(290, 290, 'Современная поэзия', 'Modern Poetry', 274, 160, 'poetry_modern');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(291, 291, 'Современная русская поэзия', 'Modern Russian Poetry', 274, 170, 'poetry_rus_modern');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(292, 292, 'Стихи о войне', 'Military Poetry', 274, 180, 'poetry_military');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(293, 293, 'Экспериментальная поэзия', 'Experimental Poetry', 274, 190, 'experimental_poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(294, 294, 'Эпическая поэзия', 'Epic Poetry', 274, 200, 'epic_poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(295, 295, 'Приключения', 'Adventure', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(296, 296, 'Авантюрный роман', 'Adventure story', 295, 10, 'adv_story');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(297, 297, 'Вестерны', 'Western', 295, 20, 'adv_western');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(298, 298, 'Военные приключения', 'Military Adventure', 295, 30, 'adv_military');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(299, 299, 'Зарубежная приключенческая литература', 'Foreign Adventure', 295, 40, 'foreign_adventure');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(300, 300, 'Исторические приключения', 'History Fiction', 295, 50, 'adv_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(301, 301, 'Морские приключения', 'Maritime Fiction', 295, 60, 'adv_maritime');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(302, 302, 'Приключения в современном мире', 'Adventure Modern', 295, 70, 'adv_modern');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(303, 303, 'Приключения про индейцев', 'Indians', 295, 80, 'adv_indian');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(304, 304, 'Приключения', 'Adventure Other', 295, 90, 'adventure');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(305, 305, 'Природа и животные', 'Nature & Animals', 295, 100, 'adv_animal');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(306, 306, 'Путешествия и география', 'Travel & Geography', 295, 110, 'adv_geo');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(307, 307, 'Русская приключенческая литература', 'Russian Adventure', 295, 120, 'adv_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(308, 308, 'Рыцарский роман', 'Tale Chivalry', 295, 130, 'tale_chivalry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(309, 309, 'Советская приключенческая литература', 'Soviet Adventure', 295, 140, 'adv_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(310, 310, 'Проза', 'Prose', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(311, 311, 'Афоризмы и цитаты', 'Aphorisms', 310, 10, 'aphorisms');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(312, 312, 'В прозе', 'In Prose', 310, 20, 'in_prose');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(313, 313, 'Военная проза', 'Military Prose', 310, 30, 'prose_military');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(314, 314, 'Зарубежная классическая проза', 'Foreign Classical Prose', 310, 40, 'foreign_prose');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(315, 315, 'Зарубежная современная проза', 'Foreign Contemporary Prose', 310, 50, 'foreign_contemporary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(316, 316, 'Историческая проза', 'Historical Prose', 310, 60, 'prose_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(317, 317, 'Классическая проза', 'Classics Prose', 310, 70, 'prose_classic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(318, 318, 'Контркультура', 'Counterculture', 310, 80, 'prose_counter');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(319, 319, 'Магический реализм', 'Magic Realism', 310, 90, 'prose_magic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(320, 320, 'Проза', 'Prose', 310, 100, 'prose');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(321, 321, 'Русская классическая проза', 'Russian Classics', 310, 110, 'prose_rus_classic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(322, 322, 'Русская современная проза', 'Russian Contemporary Prose', 310, 120, 'russian_contemporary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(323, 323, 'Семейная сага', 'Saga', 310, 130, 'sagas');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(324, 324, 'Сентиментальная проза', 'Sentimental Prose', 310, 140, 'prose_sentimental');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(325, 325, 'Советская проза', 'Soviet Classics', 310, 150, 'prose_su_classics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(326, 326, 'Современная проза', 'Contemporary Prose', 310, 160, 'prose_contemporary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(327, 327, 'Фантасмагория, абсурдистская проза', 'Phantasmagoria, Absurdist Prose', 310, 170, 'prose_abs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(328, 328, 'Феерия', 'Extravaganza', 310, 180, 'extravaganza');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(329, 329, 'Экспериментальная, неформатная проза', 'Experimental, Informal Prose', 310, 190, 'prose_neformatny');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(330, 330, 'Эпистолярная проза', 'Epistolary Fiction', 310, 200, 'epistolary_fiction');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(331, 331, 'Эпопея', 'Epic', 310, 210, 'prose_epic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(332, 332, 'Прочее', 'Other', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(333, 333, 'Антисоветская литература', 'Anti-Soviet Literature', 332, 10, 'dissident');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(334, 334, 'Бестселлеры', 'Bestsellers', 332, 20, 'bestseller');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(335, 335, 'В пересказе, в лит. обработке', 'In retelling', 332, 30, 'in_retelling');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(336, 336, 'В сокращении', 'In reduction', 332, 40, 'in_reduction');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(337, 337, 'Диафильмы', 'Diafilm', 332, 50, 'diafilm');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(338, 338, 'Для взрослых', 'For Adults', 332, 60, 'adult');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(339, 339, 'Для людей с ограниченными возможностями', 'For Invalids', 332, 70, 'invalid');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(340, 340, 'Дополнительные материалы к книге', 'Addition', 332, 80, 'addition');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(341, 341, 'Книга-игра', 'Book-game', 332, 90, 'prose_game');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(342, 342, 'Комиксы', 'Comics', 332, 100, 'comics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(343, 343, 'Литературные сказки', 'Literary Fairy Tales', 332, 110, 'literary_fairy_tale');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(344, 344, 'Любительские переводы', 'Fan Translations', 332, 120, 'fan_translation');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(345, 345, 'Начинающие авторы', 'Beginning Authors', 332, 130, 'beginning_authors');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(346, 346, 'Недописанное', 'Unfinished', 332, 140, 'unfinished');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(347, 347, 'Неотсортированное', 'Not Sorting', 332, 150, 'other');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(348, 348, 'Новеллизации', 'Novelization', 332, 160, 'novelization');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(349, 349, 'Отрывок, ознакомительный фрагмент', 'Fragment', 332, 170, 'fragment');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(350, 350, 'Презентация', 'Presentation', 332, 180, 'presentation');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(351, 351, 'Устаревшие материалы', 'Outdated', 332, 190, 'outdated');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(352, 352, 'Фанфики', 'Fan Fictions', 332, 200, 'fanfiction');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(353, 353, 'Психология и психиатрия', 'Psychology and Psychiatry', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(354, 354, 'Гипноз, внушение и самовнушение', 'Hypnosis', 353, 10, 'sci_hypnosis');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(355, 355, 'Детская психология', 'Child Psychology', 353, 20, 'psy_childs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(356, 356, 'Психиатрия и наркология', 'Psychiatry and Narcology', 353, 30, 'sci_psychiatry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(357, 357, 'Психология', 'Psychology', 353, 40, 'sci_psychology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(358, 358, 'Психология труда, инж. психология и эргономика', 'Labor Psychology, Engin. Psychology and Ergonomics', 353, 50, 'sci_tech_ergonomics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(359, 359, 'Психотерапия и консультирование', 'Psychotherapy', 353, 60, 'psy_theraphy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(360, 360, 'Секс и семейная психология', 'Sex and Family', 353, 70, 'psy_sex_and_family');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(361, 361, 'Религия и духовность', 'Religion & Inspiration', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(362, 362, 'Астрология', 'Astrology', 361, 10, 'astrology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(363, 363, 'Атеизм', 'Atheism', 361, 20, 'atheism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(364, 364, 'Буддизм', 'Buddhism', 361, 30, 'religion_budda');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(365, 365, 'Индуизм', 'Hinduism', 361, 40, 'religion_hinduism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(366, 366, 'Ислам', 'Islam', 361, 50, 'religion_islam');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(367, 367, 'Иудаизм', 'Judaism', 361, 60, 'religion_judaism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(368, 368, 'Католицизм', 'Catholicism', 361, 70, 'religion_catholicism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(369, 369, 'Православие', 'Orthodoxy', 361, 80, 'religion_orthodoxy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(370, 370, 'Протестантизм', 'Protestantism', 361, 90, 'religion_protestantism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(371, 371, 'Религиоведение', 'Religious Studies', 361, 100, 'sci_religion');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(372, 372, 'Религия', 'Religion', 361, 110, 'religion_rel');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(373, 373, 'Религия и духовность: прочее', 'Religion Other', 361, 120, 'religion');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(374, 374, 'Самосовершенствование', 'Self-improvement', 361, 130, 'religion_self');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(375, 375, 'Хиромантия', 'Palmistry', 361, 140, 'palmistry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(376, 376, 'Христианство', 'Christianity', 361, 150, 'religion_christianity');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(377, 377, 'Эзотерика, мистицизм, оккультизм', 'Esoterics', 361, 160, 'religion_esoterics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(378, 378, 'Язычество', 'Paganism', 361, 170, 'religion_paganism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(379, 379, 'Справочная литература', 'Reference', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(380, 380, 'Путеводители', 'Guides', 379, 10, 'geo_guides');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(381, 381, 'Руководства', 'Guidebooks', 379, 20, 'ref_guide');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(382, 382, 'Самоучители', 'Self-tutors', 379, 30, 'ref_self_tutor');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(383, 383, 'Словари', 'Dictionaries', 379, 40, 'ref_dict');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(384, 384, 'Справочная литература: прочее', 'Reference Other', 379, 50, 'reference');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(385, 385, 'Справочники', 'Reference', 379, 60, 'ref_ref');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(386, 386, 'Энциклопедии', 'Encyclopedias', 379, 70, 'ref_encyc');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(387, 387, 'Старинное', 'Antique Literature', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(388, 388, 'Античная литература', 'Antique', 387, 10, 'antique_ant');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(389, 389, 'Древневосточная литература', 'Old East', 387, 20, 'antique_east');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(390, 390, 'Древнерусская литература', 'Old Russian', 387, 30, 'antique_russian');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(391, 391, 'Древнеевропейская литература', 'European', 387, 40, 'antique_european');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(392, 392, 'Старинная литература', 'Other', 387, 50, 'antique');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(393, 393, 'Техника', 'Technics', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(394, 394, 'Автодело', 'Auto Business', 393, 10, 'auto_business');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(395, 395, 'Автоматизация, приборостроение', 'Instrument-Making Industry', 393, 20, 'sci_instrumentation');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(396, 396, 'Аэро-, газо- и гидродинамика', 'Aerodynamics', 393, 30, 'sci_aerodynamics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(397, 397, 'Безопасность жизнедеятельности и охрана труда', 'Safety of vital activity and labor protection', 393, 40, 'sci_tech_protection');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(398, 398, 'Гидравлика, пневматика', 'Hydraulics', 393, 50, 'sci_tech_hydraulics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(399, 399, 'История техники', 'Equ History', 393, 60, 'equ_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(400, 400, 'Конструирование, изобретательство, рационализаторство', 'Engineering', 393, 70, 'sci_engineering');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(401, 401, 'Материаловедение, конструкционные и прочие материалы', 'Materials', 393, 80, 'sci_tech_materials');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(402, 402, 'Машиностроение и металлообработка', 'Machinery', 393, 90, 'sci_tech_machinery');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(403, 403, 'Металлургия', 'Metallurgy', 393, 100, 'sci_metal');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(404, 404, 'Метрология, стандартизация и сертификация', 'Metrology', 393, 110, 'sci_tech_metrology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(405, 405, 'Механика', 'Mechanics', 393, 120, 'sci_tech_mech');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(406, 406, 'Наноматериалы и нанотехнологии', 'Nano', 393, 130, 'sci_tech_nano');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(407, 407, 'Начертательная геометрия, инженерная графика, черчение', 'Drawing', 393, 140, 'sci_tech_drawing');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(408, 408, 'Нефтегазовая и угольная промышленности', 'Oil Industry', 393, 150, 'sci_tech_oil');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(409, 409, 'Нормативно-техническая документация', 'Tech Standards', 393, 160, 'sci_tech_standards');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(410, 410, 'Полиграфическое и упаковочное производство', 'Polygraphy', 393, 170, 'sci_tech_print');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(411, 411, 'Радиоэлектроника, радиотехника, связь', 'Radio', 393, 180, 'sci_radio');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(412, 412, 'Ракетостроение, ракетная и космическая техника', 'Rockets', 393, 190, 'sci_tech_rockets');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(413, 413, 'Строительная механика и сопромат', 'Strength Of Materials', 393, 200, 'sci_tech_sopromat');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(414, 414, 'Строительство', 'Building', 393, 210, 'sci_build');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(415, 415, 'Теория механизмов и машин', 'TMM', 393, 220, 'sci_tech_theormech');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(416, 416, 'Термодинамика, теплопередача, теплотехника', 'Thermodynamics', 393, 230, 'sci_thermodynamics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(417, 417, 'Технические науки', 'Technical', 393, 240, 'sci_tech');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(418, 418, 'Транспорт, авиация и флот', 'Transport', 393, 250, 'sci_transport');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(419, 419, 'Химическая и нефтехимическая промышленности', 'Chemical Industry', 393, 260, 'sci_tech_chem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(420, 420, 'Электроника, микроэлектроника, схемотехника', 'Electronics', 393, 270, 'sci_electronics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(421, 421, 'Энергетика, электротехника', 'Energy', 393, 280, 'sci_energy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(422, 422, 'Учебники и пособия', 'Textbook', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(423, 423, 'Диссертации, дипломные, курсовые и прочие работы', 'Thesis', 422, 10, 'sci_thesis');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(424, 424, 'Советские учебники и пособия', 'Soviet Textbook', 422, 20, 'sci_textbook_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(425, 425, 'Статьи и рефераты', 'Abstract', 422, 30, 'sci_abstract');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(426, 426, 'Учебники и пособия: прочее', 'Textbook', 422, 40, 'sci_textbook');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(427, 427, 'Учебники и пособия ВУЗов', 'Textbook Higher', 422, 50, 'tbg_higher');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(428, 428, 'Учебники и пособия для среднего и специального образования', 'Textbook Secondary', 422, 60, 'tbg_secondary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(429, 429, 'Школьные учебники и пособия', 'Textbook School', 422, 70, 'tbg_school');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(430, 430, 'Шпаргалки', 'Cribs', 422, 80, 'sci_crib');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(431, 431, 'Фантастика', 'Science Fiction', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(432, 432, 'Альтернативная история', 'Alternative History', 431, 10, 'sf_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(433, 433, 'Боевая фантастика', 'Action Science Fiction', 431, 20, 'sf_action');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(434, 434, 'Героическая фантастика', 'Heroic Science Fiction', 431, 30, 'sf_heroic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(435, 435, 'Детективная фантастика', 'Detective Science Fiction', 431, 40, 'sf_detective');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(436, 436, 'Зарубежная фантастика', 'Foreign Science Fiction', 431, 50, 'foreign_sf');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(437, 437, 'Ироническая фантастика', 'Ironical Science Fiction', 431, 60, 'sf_irony');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(438, 438, 'Киберпанк', 'Cyberpunk', 431, 70, 'sf_cyberpunk');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(439, 439, 'Космическая опера', 'Space Opera', 431, 80, 'sf_space_opera');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(440, 440, 'Космическая фантастика', 'Space Science Fiction', 431, 90, 'sf_space');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(441, 441, 'ЛитРПГ', 'LitRPG', 431, 100, 'sf_litrpg');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(442, 442, 'Мистика', 'Mystic', 431, 110, 'sf_mystic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(443, 443, 'Научная фантастика', 'Science Fiction', 431, 120, 'sf');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(444, 444, 'Ненаучная фантастика', 'Non Science Fiction', 431, 130, 'nsf');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(445, 445, 'Палеонтологическая фантастика', 'Paleontological Science Fiction', 431, 140, 'sf_paleontological');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(446, 446, 'Попаданцы', 'Popadanec', 431, 150, 'popadanec');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(447, 447, 'Постапокалипсис', 'Postapocalyptic', 431, 160, 'sf_postapocalyptic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(448, 448, 'РеалРПГ', 'RealRPG', 431, 170, 'sf_realrpg');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(449, 449, 'Российская фантастика', 'Russian Science Fiction', 431, 180, 'sf_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(450, 450, 'Советская фантастика', 'Soviet Science Fiction', 431, 190, 'sf_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(451, 451, 'Социально-философская фантастика', 'Social-philosophical', 431, 200, 'sf_social');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(452, 452, 'Стимпанк', 'Stimpank', 431, 210, 'sf_stimpank');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(453, 453, 'Ужасы', 'Horror', 431, 220, 'sf_horror');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(454, 454, 'Фантастика: прочее', 'Other Science Fiction', 431, 230, 'sf_etc');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(455, 455, 'Хроноопера', 'Hronoopera', 431, 240, 'hronoopera');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(456, 456, 'Эпическая фантастика', 'Epic Science Fiction', 431, 250, 'sf_epic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(457, 457, 'Юмористическая фантастика', 'Humor Science Fiction', 431, 260, 'sf_humor');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(458, 458, 'Фольклор', 'Folklore', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(459, 459, 'Былины', 'Epic', 458, 10, 'epic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(460, 460, 'Загадки', 'Riddles', 458, 20, 'riddles');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(461, 461, 'Мифы, легенды, эпос', 'Myths, Legends, Epos', 458, 30, 'antique_myths');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(462, 462, 'Народные песни', 'Folk Songs', 458, 40, 'folk_songs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(463, 463, 'Народные приметы, обряды, традиции', 'Folk Traditions', 458, 50, 'folk_traditions');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(464, 464, 'Народные сказки', 'Folk Tales', 458, 60, 'folk_tale');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(465, 465, 'Пословицы и поговорки', 'Proverbs', 458, 70, 'proverbs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(466, 466, 'Русский фольклор', 'Russian Folklore', 458, 80, 'folklore_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(467, 467, 'Фольклор: прочее', 'Folklore', 458, 90, 'folklore');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(468, 468, 'Частушки, прибаутки, потешки', 'Limerick', 458, 100, 'limerick');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(469, 469, 'Формы произведений', 'Forms', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(470, 470, 'Авторские сборники, собрания сочинений', 'Autor Collections', 469, 10, 'autor_collection');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(471, 471, 'Газеты и журналы', 'Periodic', 469, 20, 'periodic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(472, 472, 'Компиляции', 'Compilation', 469, 30, 'compilation');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(473, 473, 'Наборы открыток', 'Postcards', 469, 40, 'postcards');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(474, 474, 'Новелла', 'Story', 469, 50, 'story');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(475, 475, 'Партитуры', 'Notes', 469, 60, 'notes');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(476, 476, 'Повесть', 'Great Story', 469, 70, 'great_story');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(477, 477, 'Рассказ', 'Short Story', 469, 80, 'short_story');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(478, 478, 'Роман', 'Roman', 469, 90, 'roman');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(479, 479, 'Сборники, альманахи, антологии', 'Collections', 469, 100, 'collection');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(480, 480, 'Статья', 'Article', 469, 110, 'article');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(481, 481, 'Эссе, очерк, этюд, набросок', 'Essay', 469, 120, 'essay');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(482, 482, 'Фэнтези', 'Fantasy', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(483, 483, 'Боевое фэнтези', 'Fight Fantasy', 482, 10, 'fantasy_fight');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(484, 484, 'Героическое фэнтези', 'Heroic Fantasy', 482, 20, 'fantasy_heroic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(485, 485, 'Городское фэнтези', 'Fantasy City', 482, 30, 'sf_fantasy_city');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(486, 486, 'Готический роман', 'Gothic Novel', 482, 40, 'gothic_novel');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(487, 487, 'Зарубежное фэнтези', 'Foreign Fantasy', 482, 50, 'foreign_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(488, 488, 'Ироническое фэнтези', 'Ironyc Female Fantasy', 482, 60, 'sf_fantasy_irony');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(489, 489, 'Историческое фэнтези', 'Historical Fantasy', 482, 70, 'historical_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(490, 490, 'Магическое фэнтези', 'Magical Fantasy', 482, 80, 'magician_book');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(491, 491, 'О вампирах', 'Vampire Fantasy', 482, 90, 'vampire_book');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(492, 492, 'О драконах', 'Dragon Fantasy', 482, 100, 'dragon_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(493, 493, 'Приключенческое фэнтези', 'Adventure Fantasy', 482, 110, 'adventure_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(494, 494, 'Российское фэнтези', 'Russian Fantasy', 482, 120, 'fantasy_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(495, 495, 'Сказочная фантастика', 'Fairy Fantasy', 482, 130, 'fairy_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(496, 496, 'Славянское фэнтези', 'Slavic Fantasy', 482, 140, 'russian_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(497, 497, 'Современная сказка', 'Modern Tale', 482, 150, 'modern_tale');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(498, 498, 'Темное фэнтези', 'Dark Fantasy', 482, 160, 'fantasy_dark');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(499, 499, 'Технофэнтези', 'Technofantasy', 482, 170, 'sf_technofantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(500, 500, 'Уся', 'Wuxia', 482, 180, 'fantasy_wuxia');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(501, 501, 'Фэнтези: прочее', 'Fantasy', 482, 190, 'sf_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(502, 502, 'Эпическое фэнтези', 'Epic Fantasy', 482, 200, 'fantasy_epic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(503, 503, 'Эротическое фэнтези', 'Erotic Fantasy', 482, 210, 'erotic_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(504, 504, 'Юмористическое фэнтези', 'Humor Fantasy', 482, 220, 'humor_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(505, 505, 'Хобби и ремесла', 'Hobbies & Crafts', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(506, 506, 'Авто-, мото- и велотранспорт, ПДД', 'Auto Regulations', 505, 10, 'auto_regulations');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(507, 507, 'Аквариумистика', 'Aquarium', 505, 20, 'home_aquarium');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(508, 508, 'Альпинизм и скалолазание', 'Mountaineering', 505, 30, 'home_mountain');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(509, 509, 'Библиотечное и редакционно-издательское дело', 'Publishing', 505, 40, 'publishing');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(510, 510, 'Виноделие, спиртные напитки', 'Winemaking', 505, 50, 'home_winemaking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(511, 511, 'Животноводство и птицеводство', 'Livestock Farming', 505, 60, 'home_livestock');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(512, 512, 'Изготовление и ремонт мебели', 'Furniture', 505, 70, 'home_furniture');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(513, 513, 'Инвентарь, инструменты', 'Inventory, Instruments', 505, 80, 'home_inventory');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(514, 514, 'Индивидуальное строительство и ремонт', 'Building and Renovation', 505, 90, 'home_building');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(515, 515, 'Книгоделие', 'Bookmaking', 505, 100, 'home_bookmaking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(516, 516, 'Коллекционирование', 'Collecting', 505, 110, 'home_collecting');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(517, 517, 'Моделизм', 'Modelling', 505, 120, 'home_modelling');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(518, 518, 'Морское дело, парусный спорт', 'Maritime Affairs', 505, 130, 'home_marine');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(519, 519, 'Охота и охотоведение', 'Hunt', 505, 140, 'home_hunt');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(520, 520, 'Писательское искусство', 'Writing Art', 505, 150, 'home_writing_art');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(521, 521, 'Пчеловодство', 'Beekeeping', 505, 160, 'home_beekeeping');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(522, 522, 'Работа по дереву', 'Woodwork', 505, 170, 'home_woodwork');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(523, 523, 'Работа по металлу', 'Metal Work', 505, 180, 'home_metalwork');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(524, 524, 'Рукоделие', 'Handiwork', 505, 190, 'home_handiwork');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(525, 525, 'Рыболовство и рыбоводство', 'Fishing', 505, 200, 'home_fishing');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(526, 526, 'Сад и огород', 'Garden', 505, 210, 'home_garden');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(527, 527, 'Сбор и выращивание грибов', 'Mushrooms', 505, 220, 'home_mushrooms');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(528, 528, 'Сделай сам', 'Do It Yourself', 505, 230, 'home_diy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(529, 529, 'Спорт', 'Sports', 505, 240, 'home_sport');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(530, 530, 'Туризм', 'Tourism', 505, 250, 'home_tourism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(531, 531, 'Хобби и ремесла: прочее', 'Hobbies & Crafts Other', 505, 260, 'home_crafts');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(532, 532, 'Цветоводство и комнатное садоводство', 'Floriculture', 505, 270, 'home_floriculture');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(533, 533, 'Юмор', 'Humor', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(534, 534, 'Анекдоты', 'Anecdote', 533, 10, 'humor_anecdote');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(535, 535, 'Байки', 'Tales', 533, 20, 'humor_tales');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(536, 536, 'Пародии, шаржи, эпиграммы', 'Parodies, Cartoons, Epigrams', 533, 30, 'humor_parody');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(537, 537, 'Сатира', 'Satire', 533, 40, 'humor_satire');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(538, 538, 'Юмор: прочее', 'Humor Other', 533, 50, 'humor');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(539, 539, 'Юмористическая проза', 'Humor Prose', 533, 60, 'humor_prose');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(540, 540, 'Юмористические стихи', 'Humor Verses', 533, 70, 'humor_verse');

/* Table data [objects_without_data] Record count: 0 */

/* Table data [params] Record count: 1 */
INSERT INTO [params]([rowid], [id], [name], [value]) VALUES(1, 1, 'version', '8');

/* Commit transaction */
COMMIT;

/* Enable foreign keys */
PRAGMA foreign_keys = 'on';
