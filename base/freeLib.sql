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
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(58, 58, 'Детское фэнтези', 'Childrens Fantasy', 49, 90, 'child_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(59, 59, 'Детский фольклор', 'Child Folklore', 49, 100, 'child_folklore');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(60, 60, 'Зарубежная детская литература', 'Foreign Literature For Children', 49, 110, 'foreign_children');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(61, 61, 'Классическая детская литература', 'Classical Childrens Literature', 49, 120, 'child_classical');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(62, 62, 'Подростковая литература', 'Young-adult Fiction', 49, 130, 'ya');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(63, 63, 'Русская детская литература', 'Russian Childrens Literature', 49, 140, 'child_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(64, 64, 'Русские сказки для детей', 'Russian Fairy Tales', 49, 150, 'child_tale_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(65, 65, 'Сказки для детей', 'Fairy Tales', 49, 160, 'child_tale');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(66, 66, 'Советская детская литература', 'Soviet Childrens Literature', 49, 170, 'child_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(67, 67, 'Документальная литература', 'Nonfiction', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(68, 68, 'Беседы и интервью', 'Interview', 67, 10, 'interview');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(69, 69, 'Биографии и мемуары', 'Biography & Memoirs', 67, 20, 'nonf_biography');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(70, 70, 'Документальная литература', 'Nonfiction Other', 67, 30, 'nonfiction');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(71, 71, 'Заметки натуралиста', 'Naturalist Notes', 67, 40, 'naturalist_notes');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(72, 72, 'Записки путешественника', 'Travel Notes', 67, 50, 'travel_notes');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(73, 73, 'Научпоп', 'Science Popular', 67, 60, 'sci_popular');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(74, 74, 'Публицистика', 'Publicism', 67, 70, 'nonf_publicism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(75, 75, 'Домоводство (Дом и семья)', 'Home & Family', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(76, 76, 'Выживание и личная безопасность', 'Survival', 75, 10, 'home_survival');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(77, 77, 'Дети. Книги для родителей', 'Child', 75, 20, 'home_child');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(78, 78, 'Дом и семья: прочее', 'Home Other', 75, 30, 'home');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(79, 79, 'Домашние животные', 'Pets', 75, 40, 'home_pets');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(80, 80, 'Домоводство', 'Housekeeping', 75, 50, 'home_housekeeping');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(81, 81, 'Здоровье', 'Health', 75, 60, 'home_health');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(82, 82, 'Красота', 'Beauty', 75, 70, 'home_beauty');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(83, 83, 'Кулинария', 'Cooking', 75, 80, 'home_cooking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(84, 84, 'Развлечения', 'Entertaining', 75, 90, 'home_entertain');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(85, 85, 'Семейные отношения', 'Family', 75, 100, 'family');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(86, 86, 'Эротика и секс', 'Erotica & Sex', 75, 110, 'home_sex');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(87, 87, 'Драматургия', 'Dramaturgy', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(88, 88, 'Античная драма', 'Antique Drama', 87, 10, 'drama_antique');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(89, 89, 'Водевиль, буффонада', 'Vaudeville', 87, 20, 'vaudeville');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(90, 90, 'Драма', 'Drama', 87, 30, 'drama');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(91, 91, 'Драматургия', 'Dramaturgy', 87, 40, 'dramaturgy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(92, 92, 'Зарубежная классическая драматургия', 'Foreign Dramaturgy', 87, 50, 'dramaturgy_for_classic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(93, 93, 'Зарубежная современная драматургия', 'Contemporary Foreign Dramaturgy', 87, 60, 'foreign_dramaturgy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(94, 94, 'Киносценарий', 'Screenplays', 87, 70, 'screenplays');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(95, 95, 'Комедия', 'Comedy', 87, 80, 'comedy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(96, 96, 'Мистерия', 'Mystery', 87, 90, 'mystery');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(97, 97, 'Русская классическая драматургия', 'Russian Dramaturgy', 87, 100, 'dramaturgy_rus_classic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(98, 98, 'Русская современная драматургия', 'Contemporary Russian Dramaturgy', 87, 110, 'dramaturgy_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(99, 99, 'Советская драматургия', 'Soviet Dramaturgy', 87, 120, 'dramaturgy_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(100, 100, 'Сценарий', 'Scenarios', 87, 130, 'scenarios');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(101, 101, 'Трагедия', 'Tragedy', 87, 140, 'tragedy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(102, 102, 'История', 'History', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(103, 103, 'История', 'History', 102, 10, 'sci_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(104, 104, 'История первобытного общества', 'History of Primitive Society', 102, 20, 'sci_history_0');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(105, 105, 'История древнего мира', 'Ancient World History', 102, 30, 'sci_history_4');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(106, 106, 'История средних веков', 'Medieval History', 102, 40, 'sci_history_15');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(107, 107, 'История нового времени', 'History of Modern Times', 102, 50, 'sci_history_18');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(108, 108, 'Новейшая история', 'Newest History', 102, 60, 'sci_history_20');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(109, 109, 'Современная история', 'Contemporary History', 102, 70, 'sci_history_21');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(110, 110, 'Компьютеры и Интернет', 'Computers & Internet', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(111, 111, 'CAD, CAM и CAE системы', 'CAD, CAM and CAE', 110, 10, 'comp_soft_cad');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(112, 112, 'Web-дизайн', 'Web Design', 110, 20, 'comp_www_design');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(113, 113, 'Базы данных', 'Databases', 110, 30, 'comp_db');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(114, 114, 'Графика. Дизайн. Мультимедиа', 'Design', 110, 40, 'comp_design');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(115, 115, 'Интернет', 'Internet', 110, 50, 'comp_www');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(116, 116, 'История информатики и вычислительной техники', 'Computers History', 110, 60, 'comp_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(117, 117, 'Компьютерная безопасность', 'Security', 110, 70, 'comp_security');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(118, 118, 'Компьютерное железо', 'Hardware', 110, 80, 'comp_hard');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(119, 119, 'Околокомпьютерная литература', 'Computers Other', 110, 90, 'computers');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(120, 120, 'Офисные приложения', 'Office Software', 110, 100, 'comp_soft_office');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(121, 121, 'Программы', 'Software', 110, 110, 'comp_soft');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(122, 122, 'Сети', 'Networking', 110, 120, 'comp_osnet');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(123, 123, 'Учебники и самоучители по компьютеру', 'Computers Textbook', 110, 130, 'tbg_computers');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(124, 124, 'Хакерство', 'Hacking', 110, 140, 'comp_hacking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(125, 125, 'Цифровая обработка сигналов', 'Digital Signal Processing', 110, 150, 'comp_dsp');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(126, 126, 'Компьютеры: Операционные системы', 'OS', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(127, 127, 'Android', 'Android', 126, 10, 'comp_os_android');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(128, 128, 'FreeDOS', 'FreeDOS', 126, 20, 'comp_os_freedos');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(129, 129, 'Linux', 'Linux', 126, 30, 'comp_os_linux');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(130, 130, 'MacOS', 'MacOS', 126, 40, 'comp_os_macos');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(131, 131, 'MS-DOS', 'MS-DOS', 126, 50, 'comp_os_msdos');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(132, 132, 'OS/2', 'OS/2', 126, 60, 'comp_os_os2');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(133, 133, 'Unix', 'Unix', 126, 70, 'comp_os_unix');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(134, 134, 'Windows', 'Windows', 126, 80, 'comp_os_windows');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(135, 135, 'ОС: теоретические вопросы', 'OS Theory', 126, 90, 'comp_os_theory');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(136, 136, 'Прочие ОС', 'Other OS', 126, 100, 'comp_os');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(137, 137, 'Компьютеры: Разработка ПО', 'Soft Development', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(138, 138, 'Алгоритмы и структуры данных', 'Algorithms', 137, 10, 'comp_soft_dev_alg');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(139, 139, 'Базы знаний и экспертные системы', 'Expert Systems', 137, 20, 'comp_db_exp');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(140, 140, 'Искусственный интеллект', 'Artifical Intellegence', 137, 30, 'comp_dv_ai');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(141, 141, 'Крэкинг и реверсинжиниринг', 'Craking And Reverse Engineering', 137, 40, 'comp_soft_dev_craking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(142, 142, 'Менеджмент ПО', 'Management', 137, 50, 'comp_soft_dev_man');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(143, 143, 'Объектно-ориентированное программирование', 'OOP', 137, 60, 'comp_soft_dev_oop');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(144, 144, 'Отладка и тестирование ПО', 'Debuging', 137, 70, 'comp_soft_dev_debug');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(145, 145, 'Параллельное и распределенное программирование', 'Parallel Programming', 137, 80, 'comp_soft_dev_parallel');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(146, 146, 'Программирование графики', 'Graphic Programming', 137, 90, 'comp_soft_dev_graphic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(147, 147, 'Программирование игр', 'Games Programming', 137, 100, 'comp_soft_dev_games');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(148, 148, 'Программирование: прочее', 'Programming Other', 137, 110, 'comp_soft_dev');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(149, 149, 'Системное программирование', 'System Programming', 137, 120, 'comp_soft_dev_system');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(150, 150, 'Компьютеры: Языки и системы программирования', 'Programming Languages', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(151, 151, '.NET Framework', '.NET Framework', 150, 10, 'comp_prog_dotnet');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(152, 152, 'Ada', 'Ada', 150, 20, 'comp_prog_ada');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(153, 153, 'Assembler', 'Assembler', 150, 30, 'comp_prog_assembler');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(154, 154, 'Basic, Visual Basic, VB Script, VBA и т.п.', 'Basic, Visual Basic, VB Script, VBA', 150, 40, 'comp_prog_basic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(155, 155, 'C, C++, C#', 'C, C++, C#', 150, 50, 'comp_prog_c');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(156, 156, 'Forth', 'Forth', 150, 60, 'comp_prog_forth');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(157, 157, 'Fortran', 'Fortran', 150, 70, 'comp_prog_fortran');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(158, 158, 'Java, Java Script', 'Java, Java Script', 150, 80, 'comp_prog_java');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(159, 159, 'Lisp', 'Lisp', 150, 90, 'comp_prog_lisp');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(160, 160, 'Lua', 'Lua', 150, 100, 'comp_prog_lua');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(161, 161, 'MFC', 'MFC', 150, 110, 'comp_prog_mfc');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(162, 162, 'Modula-2, Modula-3, Oberon, Oberon-2', 'Modula-2, Modula-3, Oberon, Oberon-2', 150, 120, 'comp_prog_oberon');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(163, 163, 'Pascal, Delphi, Lazarus и т.п.', 'Pascal, Delphi, Lazarus', 150, 130, 'comp_prog_pascal');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(164, 164, 'PHP', 'PHP', 150, 140, 'comp_prog_php');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(165, 165, 'Prolog', 'Prolog', 150, 150, 'comp_prog_prolog');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(166, 166, 'Python', 'Python', 150, 160, 'comp_prog_python');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(167, 167, 'Qt', 'Qt', 150, 170, 'comp_prog_qt');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(168, 168, 'Ruby', 'Ruby', 150, 180, 'comp_prog_ror');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(169, 169, 'Windows API', 'Windows API', 150, 190, 'comp_prog_winapi');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(170, 170, 'Другие языки и системы программирования', 'Other Programming Languages', 150, 200, 'comp_programming');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(171, 171, 'Культура и искусство', 'Art, Art History, Design', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(172, 172, 'Архитектура и скульптура', 'Architecture', 171, 10, 'architecture_book');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(173, 173, 'Живопись, альбомы, иллюстрированные каталоги', 'Painting', 171, 20, 'painting');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(174, 174, 'Изобразительное искусство, фотография', 'Visual Arts', 171, 30, 'visual_arts');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(175, 175, 'Искусство и дизайн', 'Art, Design', 171, 40, 'design');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(176, 176, 'Искусствоведение', 'Art Criticism', 171, 50, 'art_criticism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(177, 177, 'История искусства', 'Art History', 171, 60, 'art_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(178, 178, 'Кино', 'Cine', 171, 70, 'cine');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(179, 179, 'Критика', 'Criticism', 171, 80, 'nonf_criticism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(180, 180, 'Культурология', 'Cultural Science', 171, 90, 'sci_culture');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(181, 181, 'Мировая художественная культура', 'Art World Culture', 171, 100, 'art_world_culture');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(182, 182, 'Мода и стиль', 'Fashion Style', 171, 110, 'fashion_style');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(183, 183, 'Музыка', 'Music', 171, 120, 'music');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(184, 184, 'Партитуры', 'Notes', 171, 130, 'notes');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(185, 185, 'Радио и телевидение', 'Radio and TV', 171, 140, 'radio_tv');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(186, 186, 'Танцы и хореография', 'Dance and Choreography', 171, 150, 'art_dance');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(187, 187, 'Театр', 'Theatre', 171, 160, 'theatre');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(188, 188, 'Литература по изданиям', 'Publications', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(189, 189, 'Дореволюционные зарубежные издания', 'Pre-revolutionary Foreign Publications', 188, 10, 'old_foreign_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(190, 190, 'Дореволюционные российские издания', 'Pre-revolutionary Russian Publications', 188, 20, 'old_rus_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(191, 191, 'Зарубежные издания советского периода', 'Foreign publications of the Soviet period', 188, 30, 'foreign_su_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(192, 192, 'Раритетные издания', 'Rare Editions', 188, 40, 'rarity');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(193, 193, 'Самиздат, сетевая литература', 'Network Literature', 188, 50, 'network_literature');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(194, 194, 'Советские издания', 'Soviet Publications', 188, 60, 'su_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(195, 195, 'Современные зарубежные издания', 'Modern Foreign Publications', 188, 70, 'foreign_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(196, 196, 'Современные издания стран бывшего СССР', 'Publications Of The Former USSR', 188, 80, 'ex_su_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(197, 197, 'Современные российские издания', 'Modern Russian Publications', 188, 90, 'rus_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(198, 198, 'Литература по эпохам', 'Literature Ages', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(199, 199, 'Литература IV века и ранее (эпоха Древнего мира)', 'Literature Before The V Century', 198, 10, 'literature_4');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(200, 200, 'Литература V-XIII веков (эпоха Средневековья)', 'Literature Of V-XIII Centuries', 198, 20, 'foreign_antique');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(201, 201, 'Литература XIV-XVI веков (эпоха Возрождения)', 'Literature Of XIV-XVI Centuries', 198, 30, 'literature_16');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(202, 202, 'Литература XVII-XVIII веков (эпоха Просвящения)', 'Literature Of XVII-XVIII Centuries', 198, 40, 'literature_18');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(203, 203, 'Литература ХIX века (эпоха Промышленной революции)', 'Literature Of The XIX Century', 198, 50, 'literature_19');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(204, 204, 'Литература ХX века (эпоха Социальных революций)', 'Literature Of The ХX Century', 198, 60, 'literature_20');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(205, 205, 'Литература ХXI века (эпоха Глобализации экономики)', 'Literature Of The ХXI Century', 198, 70, 'literature_21');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(206, 206, 'Любовные романы', 'Romance', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(207, 207, 'Зарубежная литература о любви', 'Foreign Love Romance', 206, 10, 'foreign_love');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(208, 208, 'Исторические любовные романы', 'Historical Romance', 206, 20, 'love_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(209, 209, 'Короткие любовные романы', 'Short Romance', 206, 30, 'love_short');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(210, 210, 'Любовная фантастика', 'Love SF', 206, 40, 'love_sf');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(211, 211, 'Любовное фэнтези', 'Love Fantasy', 206, 50, 'love_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(212, 212, 'Любовные детективы', 'Detective Romance', 206, 60, 'love_detective');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(213, 213, 'О любви', 'Love Romance', 206, 70, 'love');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(214, 214, 'Порно', 'Porno', 206, 80, 'love_hard');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(215, 215, 'Русская литература о любви', 'Russian Love Romance', 206, 90, 'love_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(216, 216, 'Сексуальные извращения', 'Sexual Perversion', 206, 100, 'sexual_perversion');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(217, 217, 'Слэш', 'Love Slash', 206, 110, 'love_slash');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(218, 218, 'Советская литература о любви', 'Soviet Love Romance', 206, 120, 'love_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(219, 219, 'Современные любовные романы', 'Contemporary Romance', 206, 130, 'love_contemporary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(220, 220, 'Фемслеш', 'Love Femslash', 206, 140, 'love_femslash');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(221, 221, 'Эротика', 'Erotica', 206, 150, 'love_erotica');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(222, 222, 'Наука, образование', 'Scientific-educational', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(223, 223, 'Альтернативная медицина', 'Alternative Medicine', 222, 10, 'sci_medicine_alternative');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(224, 224, 'Альтернативные науки и научные теории', 'Theories', 222, 20, 'sci_theories');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(225, 225, 'Аналитическая химия', 'Analitic Chemistry', 222, 30, 'sci_anachem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(226, 226, 'Археология', 'Archeology', 222, 40, 'sci_archeology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(227, 227, 'Астрономия и Космос', 'Cosmos', 222, 50, 'sci_cosmos');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(228, 228, 'Ветеринария', 'Veterinary', 222, 60, 'sci_veterinary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(229, 229, 'Востоковедение', 'Science Oriental', 222, 70, 'sci_oriental');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(230, 230, 'География', 'Geography', 222, 80, 'sci_geography');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(231, 231, 'Геология и геофизика', 'Geology and Geophysics', 222, 90, 'sci_geo');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(232, 232, 'Государство и право', 'State Science', 222, 100, 'sci_state');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(233, 233, 'Иностранные языки', 'Foreign Languages', 222, 110, 'foreign_language');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(234, 234, 'История науки', 'Science History', 222, 120, 'science_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(235, 235, 'Краеведение', 'Study of Local Lore', 222, 130, 'local_lore_study');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(236, 236, 'Литературоведение', 'Philology', 222, 140, 'sci_philology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(237, 237, 'Математика', 'Mathematics', 222, 150, 'sci_math');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(238, 238, 'Медицина', 'Medicine', 222, 160, 'sci_medicine');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(239, 239, 'Научная литература', 'Science Other', 222, 170, 'science');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(240, 240, 'Обществознание', 'Social Studies', 222, 180, 'sci_social_studies');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(241, 241, 'Органическая химия', 'Органическая химия', 222, 190, 'sci_orgchem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(242, 242, 'Педагогика', 'Pedagogy', 222, 200, 'sci_pedagogy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(243, 243, 'Политика и дипломатия', 'Politics and Diplomacy', 222, 210, 'sci_politics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(244, 244, 'Религиоведение', 'Religious Studies', 222, 220, 'sci_religion');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(245, 245, 'Физика', 'Physics', 222, 230, 'sci_phys');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(246, 246, 'Физическая химия', 'Physical Chemistry', 222, 240, 'sci_physchem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(247, 247, 'Философия', 'Philosophy', 222, 250, 'sci_philosophy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(248, 248, 'Химия', 'Chemistry', 222, 260, 'sci_chem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(249, 249, 'Экономика', 'Economic', 222, 270, 'sci_economy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(250, 250, 'Юриспруденция', 'Jurisprudence', 222, 280, 'sci_juris');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(251, 251, 'Языкознание', 'Linguistics', 222, 290, 'sci_linguistic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(252, 252, 'Науки о живой природе', 'Biology', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(253, 253, 'Биология', 'Biology', 252, 10, 'sci_biology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(254, 254, 'Биофизика', 'Biophysics', 252, 20, 'sci_biophys');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(255, 255, 'Биохимия', 'Biochemistry', 252, 30, 'sci_biochem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(256, 256, 'Ботаника', 'Botany', 252, 40, 'sci_botany');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(257, 257, 'Зоология', 'Zoology', 252, 50, 'sci_zoo');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(258, 258, 'Палеонтология', 'Paleontology', 252, 60, 'sci_paleontology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(259, 259, 'Эволюционизм', 'Evolutionism', 252, 70, 'sci_evolutionism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(260, 260, 'Экология', 'Ecology', 252, 80, 'sci_ecology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(261, 261, 'Поэзия', 'Poetry', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(262, 262, 'Басни', 'Fable', 261, 10, 'fable');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(263, 263, 'В стихах', 'In Verse', 261, 20, 'in_verse');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(264, 264, 'Верлибры', 'Vers Libre', 261, 30, 'vers_libre');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(265, 265, 'Визуальная поэзия', 'Visual Poetry', 261, 40, 'visual_poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(266, 266, 'Классическая зарубежная поэзия', 'Classical Foreign Poetry', 261, 50, 'poetry_for_classical');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(267, 267, 'Классическая поэзия', 'Classical Poetry', 261, 60, 'poetry_classical');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(268, 268, 'Классическая русская поэзия', 'Classical Russian Poetry', 261, 70, 'poetry_rus_classical');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(269, 269, 'Лирика', 'Lyrics', 261, 80, 'lyrics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(270, 270, 'Палиндромы', 'Palindromes', 261, 90, 'palindromes');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(271, 271, 'Песенная поэзия', 'Song Poetry', 261, 100, 'song_poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(272, 272, 'Поэзия', 'Poetry', 261, 110, 'poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(273, 273, 'Поэзия Востока', 'Poetry Of The East', 261, 120, 'poetry_east');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(274, 274, 'Поэма', 'Poem', 261, 130, 'poem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(275, 275, 'Советская поэзия', 'Soviet Poetry', 261, 140, 'poetry_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(276, 276, 'Современная зарубежная поэзия', 'Modern Foreign Poetry', 261, 150, 'poetry_for_modern');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(277, 277, 'Современная поэзия', 'Modern Poetry', 261, 160, 'poetry_modern');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(278, 278, 'Современная русская поэзия', 'Modern Russian Poetry', 261, 170, 'poetry_rus_modern');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(279, 279, 'Стихи о войне', 'Military Poetry', 261, 180, 'poetry_military');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(280, 280, 'Экспериментальная поэзия', 'Experimental Poetry', 261, 190, 'experimental_poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(281, 281, 'Эпическая поэзия', 'Epic Poetry', 261, 200, 'epic_poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(282, 282, 'Приключения', 'Adventure', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(283, 283, 'Авантюрный роман', 'Adventure story', 282, 10, 'adv_story');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(284, 284, 'Вестерны', 'Western', 282, 20, 'adv_western');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(285, 285, 'Военные приключения', 'Military Adventure', 282, 30, 'adv_military');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(286, 286, 'Исторические приключения', 'History Fiction', 282, 40, 'adv_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(287, 287, 'Морские приключения', 'Maritime Fiction', 282, 50, 'adv_maritime');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(288, 288, 'Приключения в современном мире', 'Adventure Modern', 282, 60, 'adv_modern');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(289, 289, 'Приключения про индейцев', 'Indians', 282, 70, 'adv_indian');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(290, 290, 'Приключения', 'Adventure Other', 282, 80, 'adventure');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(291, 291, 'Природа и животные', 'Nature & Animals', 282, 90, 'adv_animal');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(292, 292, 'Путешествия и география', 'Travel & Geography', 282, 100, 'adv_geo');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(293, 293, 'Рыцарский роман', 'Tale Chivalry', 282, 110, 'tale_chivalry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(294, 294, 'Советская приключенческая литература', 'Soviet Adventure', 282, 120, 'adv_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(295, 295, 'Проза', 'Prose', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(296, 296, 'Афоризмы и цитаты', 'Aphorisms', 295, 10, 'aphorisms');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(297, 297, 'В прозе', 'In Prose', 295, 20, 'in_prose');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(298, 298, 'Военная проза', 'Military Prose', 295, 30, 'prose_military');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(299, 299, 'Зарубежная классическая проза', 'Foreign Classical Prose', 295, 40, 'foreign_prose');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(300, 300, 'Зарубежная современная проза', 'Foreign Contemporary Prose', 295, 50, 'foreign_contemporary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(301, 301, 'Историческая проза', 'Historical Prose', 295, 60, 'prose_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(302, 302, 'Классическая проза', 'Classics Prose', 295, 70, 'prose_classic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(303, 303, 'Контркультура', 'Counterculture', 295, 80, 'prose_counter');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(304, 304, 'Магический реализм', 'Magic Realism', 295, 90, 'prose_magic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(305, 305, 'Новелла', 'Story', 295, 100, 'story');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(306, 306, 'Повесть', 'Great Story', 295, 110, 'great_story');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(307, 307, 'Проза', 'Prose', 295, 120, 'prose');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(308, 308, 'Рассказ', 'Short Story', 295, 130, 'short_story');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(309, 309, 'Роман', 'Roman', 295, 140, 'roman');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(310, 310, 'Русская классическая проза', 'Russian Classics', 295, 150, 'prose_rus_classic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(311, 311, 'Русская современная проза', 'Russian Contemporary Prose', 295, 160, 'russian_contemporary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(312, 312, 'Семейная сага', 'Saga', 295, 170, 'sagas');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(313, 313, 'Сентиментальная проза', 'Sentimental Prose', 295, 180, 'prose_sentimental');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(314, 314, 'Советская проза', 'Soviet Classics', 295, 190, 'prose_su_classics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(315, 315, 'Современная проза', 'Contemporary Prose', 295, 200, 'prose_contemporary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(316, 316, 'Фантасмагория, абсурдистская проза', 'Phantasmagoria, Absurdist Prose', 295, 210, 'prose_abs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(317, 317, 'Феерия', 'Extravaganza', 295, 220, 'extravaganza');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(318, 318, 'Экспериментальная, неформатная проза', 'Experimental, Informal Prose', 295, 230, 'prose_neformatny');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(319, 319, 'Эпистолярная проза', 'Epistolary Fiction', 295, 240, 'epistolary_fiction');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(320, 320, 'Эпопея', 'Epic', 295, 250, 'prose_epic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(321, 321, 'Эссе, очерк, этюд, набросок', 'Essay', 295, 260, 'essay');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(322, 322, 'Прочее', 'Other', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(323, 323, 'Авторские сборники, собрания сочинений', 'Autor Collections', 322, 10, 'autor_collection');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(324, 324, 'Антисоветская литература', 'Anti-Soviet Literature', 322, 20, 'dissident');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(325, 325, 'Бестселлеры', 'Bestsellers', 322, 30, 'bestseller');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(326, 326, 'В пересказе, в лит. обработке', 'In retelling', 322, 40, 'in_retelling');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(327, 327, 'В сокращении', 'In reduction', 322, 50, 'in_reduction');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(328, 328, 'Газеты и журналы', 'Periodic', 322, 60, 'periodic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(329, 329, 'Диафильмы', 'Diafilm', 322, 70, 'diafilm');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(330, 330, 'Для взрослых', 'For Adults', 322, 80, 'adult');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(331, 331, 'Книга-игра', 'Book-game', 322, 90, 'prose_game');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(332, 332, 'Комиксы', 'Comics', 322, 100, 'comics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(333, 333, 'Компиляции', 'Compilation', 322, 110, 'compilation');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(334, 334, 'Кошки', 'Cats', 322, 120, 'cats');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(335, 335, 'Литературные сказки', 'Literary Fairy Tales', 322, 130, 'literary_fairy_tale');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(336, 336, 'Любительские переводы', 'Fan Translations', 322, 140, 'fan_translation');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(337, 337, 'Наборы открыток', 'Postcards', 322, 150, 'postcards');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(338, 338, 'Начинающие авторы', 'Beginning Authors', 322, 160, 'beginning_authors');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(339, 339, 'Недописанное', 'Unfinished', 322, 170, 'unfinished');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(340, 340, 'Неотсортированное', 'Not Sorting', 322, 180, 'other');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(341, 341, 'Новеллизации', 'Novelization', 322, 190, 'novelization');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(342, 342, 'Отрывок, ознакомительный фрагмент', 'Fragment', 322, 200, 'fragment');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(343, 343, 'Сборники, альманахи, антологии', 'Collections', 322, 210, 'collection');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(344, 344, 'Фанфики', 'Fan Fictions', 322, 220, 'fanfiction');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(345, 345, 'Психология и психиатрия', 'Psychology and Psychiatry', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(346, 346, 'Гипноз, внушение и самовнушение', 'Hypnosis', 345, 10, 'sci_hypnosis');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(347, 347, 'Детская психология', 'Child Psychology', 345, 20, 'psy_childs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(348, 348, 'Психиатрия и наркология', 'Psychiatry and Narcology', 345, 30, 'sci_psychiatry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(349, 349, 'Психология', 'Psychology', 345, 40, 'sci_psychology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(350, 350, 'Психотерапия и консультирование', 'Psychotherapy', 345, 50, 'psy_theraphy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(351, 351, 'Секс и семейная психология', 'Sex and Family', 345, 60, 'psy_sex_and_family');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(352, 352, 'Религия и духовность', 'Religion & Inspiration', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(353, 353, 'Астрология', 'Astrology', 352, 10, 'astrology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(354, 354, 'Атеизм', 'Atheism', 352, 20, 'atheism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(355, 355, 'Буддизм', 'Buddhism', 352, 30, 'religion_budda');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(356, 356, 'Индуизм', 'Hinduism', 352, 40, 'religion_hinduism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(357, 357, 'Ислам', 'Islam', 352, 50, 'religion_islam');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(358, 358, 'Иудаизм', 'Judaism', 352, 60, 'religion_judaism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(359, 359, 'Католицизм', 'Catholicism', 352, 70, 'religion_catholicism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(360, 360, 'Православие', 'Orthodoxy', 352, 80, 'religion_orthodoxy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(361, 361, 'Протестантизм', 'Protestantism', 352, 90, 'religion_protestantism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(362, 362, 'Религия и духовность: прочее', 'Religion Other', 352, 100, 'religion');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(363, 363, 'Религия', 'Religion', 352, 110, 'religion_rel');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(364, 364, 'Самосовершенствование', 'Self-improvement', 352, 120, 'religion_self');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(365, 365, 'Хиромантия', 'Palmistry', 352, 130, 'palmistry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(366, 366, 'Христианство', 'Christianity', 352, 140, 'religion_christianity');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(367, 367, 'Эзотерика', 'Esoterics', 352, 150, 'religion_esoterics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(368, 368, 'Язычество', 'Paganism', 352, 160, 'religion_paganism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(369, 369, 'Справочная литература', 'Reference', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(370, 370, 'Путеводители', 'Guides', 369, 10, 'geo_guides');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(371, 371, 'Руководства', 'Guidebooks', 369, 20, 'ref_guide');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(372, 372, 'Самоучители', 'Self-tutors', 369, 30, 'ref_self_tutor');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(373, 373, 'Словари', 'Dictionaries', 369, 40, 'ref_dict');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(374, 374, 'Справочная литература: прочее', 'Reference Other', 369, 50, 'reference');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(375, 375, 'Справочники', 'Reference', 369, 60, 'ref_ref');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(376, 376, 'Энциклопедии', 'Encyclopedias', 369, 70, 'ref_encyc');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(377, 377, 'Старинное', 'Antique Literature', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(378, 378, 'Античная литература', 'Antique', 377, 10, 'antique_ant');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(379, 379, 'Древневосточная литература', 'Old East', 377, 20, 'antique_east');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(380, 380, 'Древнерусская литература', 'Old Russian', 377, 30, 'antique_russian');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(381, 381, 'Древнеевропейская литература', 'European', 377, 40, 'antique_european');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(382, 382, 'Старинная литература', 'Other', 377, 50, 'antique');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(383, 383, 'Техника', 'Technics', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(384, 384, 'Автодело', 'Auto Business', 383, 10, 'auto_business');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(385, 385, 'Аэро-, газо- и гидродинамика', 'Aerodynamics', 383, 20, 'sci_aerodynamics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(386, 386, 'Гидравлика', 'Hydraulics', 383, 30, 'sci_tech_hydraulics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(387, 387, 'История техники', 'Equ History', 383, 40, 'equ_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(388, 388, 'Конструирование', 'Engineering', 383, 50, 'sci_engineering');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(389, 389, 'Материаловедение, конструкционные материалы', 'Materials', 383, 60, 'sci_tech_materials');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(390, 390, 'Машиностроение', 'Machinery', 383, 70, 'sci_tech_machinery');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(391, 391, 'Металлургия', 'Metallurgy', 383, 80, 'sci_metal');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(392, 392, 'Метрология, стандартизация и сертификация', 'Metrology', 383, 90, 'sci_tech_metrology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(393, 393, 'Нефтегазовая промышленность', 'Oil Industry', 383, 100, 'sci_tech_oil');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(394, 394, 'Нормативная техническая документация', 'Tech Standards', 383, 110, 'sci_tech_standards');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(395, 395, 'Приборостроение', 'Instrument-Making Industry', 383, 120, 'sci_instrumentation');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(396, 396, 'Радиоэлектроника, радиотехника, связь', 'Radio', 383, 130, 'sci_radio');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(397, 397, 'Ракетостроение и космическая техника', 'Rockets', 383, 140, 'sci_tech_rockets');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(398, 398, 'Строительная механика и сопромат', 'Strength Of Materials', 383, 150, 'sci_tech_sopromat');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(399, 399, 'Строительство', 'Building', 383, 160, 'sci_build');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(400, 400, 'Теория механизмов и машин', 'TMM', 383, 170, 'sci_tech_theormech');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(401, 401, 'Термодинамика и теплотехника', 'Thermodynamics', 383, 180, 'sci_thermodynamics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(402, 402, 'Техническая механика', 'Technical Mechanics', 383, 190, 'sci_tech_mech');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(403, 403, 'Технические науки', 'Technical', 383, 200, 'sci_tech');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(404, 404, 'Транспорт и авиация', 'Transport', 383, 210, 'sci_transport');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(405, 405, 'Химическая промышленность', 'Chemical Industry', 383, 220, 'sci_tech_chem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(406, 406, 'Электроника, микроэлектроника, схемотехника', 'Electronics', 383, 230, 'sci_electronics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(407, 407, 'Энергетика. Электротехника', 'Energy', 383, 240, 'sci_energy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(408, 408, 'Учебники и пособия', 'Textbook', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(409, 409, 'Диссертации, дипломные, курсовые и прочие работы', 'Thesis', 408, 10, 'sci_thesis');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(410, 410, 'Рефераты', 'Abstract', 408, 20, 'sci_abstract');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(411, 411, 'Советские учебники и пособия', 'Soviet Textbook', 408, 30, 'sci_textbook_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(412, 412, 'Учебники и пособия: прочее', 'Textbook', 408, 40, 'sci_textbook');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(413, 413, 'Учебники и пособия ВУЗов', 'Textbook Higher', 408, 50, 'tbg_higher');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(414, 414, 'Учебники и пособия для среднего и специального образования', 'Textbook Secondary', 408, 60, 'tbg_secondary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(415, 415, 'Школьные учебники и пособия', 'Textbook School', 408, 70, 'tbg_school');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(416, 416, 'Шпаргалки', 'Cribs', 408, 80, 'sci_crib');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(417, 417, 'Фантастика', 'Science Fiction', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(418, 418, 'Альтернативная история', 'Alternative History', 417, 10, 'sf_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(419, 419, 'Боевая фантастика', 'Action Science Fiction', 417, 20, 'sf_action');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(420, 420, 'Героическая фантастика', 'Heroic Science Fiction', 417, 30, 'sf_heroic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(421, 421, 'Детективная фантастика', 'Detective Science Fiction', 417, 40, 'sf_detective');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(422, 422, 'Зарубежная фантастика', 'Foreign Science Fiction', 417, 50, 'foreign_sf');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(423, 423, 'Ироническая фантастика', 'Ironical Science Fiction', 417, 60, 'sf_irony');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(424, 424, 'Киберпанк', 'Cyberpunk', 417, 70, 'sf_cyberpunk');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(425, 425, 'Космическая опера', 'Space Opera', 417, 80, 'sf_space_opera');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(426, 426, 'Космическая фантастика', 'Space Science Fiction', 417, 90, 'sf_space');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(427, 427, 'ЛитРПГ', 'LitRPG', 417, 100, 'sf_litrpg');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(428, 428, 'Мистика', 'Mystic', 417, 110, 'sf_mystic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(429, 429, 'Научная фантастика', 'Science Fiction', 417, 120, 'sf');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(430, 430, 'Ненаучная фантастика', 'Non Science Fiction', 417, 130, 'nsf');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(431, 431, 'Палеонтологическая фантастика', 'Paleontological Science Fiction', 417, 140, 'sf_paleontological');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(432, 432, 'Попаданцы', 'Popadanec', 417, 150, 'popadanec');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(433, 433, 'Постапокалипсис', 'Postapocalyptic', 417, 160, 'sf_postapocalyptic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(434, 434, 'Российская фантастика', 'Russian Science Fiction', 417, 170, 'sf_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(435, 435, 'Советская фантастика', 'Soviet Science Fiction', 417, 180, 'sf_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(436, 436, 'Социально-философская фантастика', 'Social-philosophical', 417, 190, 'sf_social');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(437, 437, 'Стимпанк', 'Stimpank', 417, 200, 'sf_stimpank');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(438, 438, 'Ужасы', 'Horror', 417, 210, 'sf_horror');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(439, 439, 'Фантастика: прочее', 'Other Science Fiction', 417, 220, 'sf_etc');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(440, 440, 'Хроноопера', 'Hronoopera', 417, 230, 'hronoopera');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(441, 441, 'Эпическая фантастика', 'Epic Science Fiction', 417, 240, 'sf_epic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(442, 442, 'Юмористическая фантастика', 'Humor Science Fiction', 417, 250, 'sf_humor');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(443, 443, 'Фольклор', 'Folklore', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(444, 444, 'Былины', 'Epic', 443, 10, 'epic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(445, 445, 'Загадки', 'Riddles', 443, 20, 'riddles');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(446, 446, 'Мифы, легенды, эпос', 'Myths, Legends, Epos', 443, 30, 'antique_myths');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(447, 447, 'Народные песни', 'Folk Songs', 443, 40, 'folk_songs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(448, 448, 'Народные приметы, обряды, традиции', 'Folk Traditions', 443, 50, 'folk_traditions');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(449, 449, 'Народные сказки', 'Folk Tales', 443, 60, 'folk_tale');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(450, 450, 'Пословицы и поговорки', 'Proverbs', 443, 70, 'proverbs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(451, 451, 'Русский фольклор', 'Russian Folklore', 443, 80, 'folklore_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(452, 452, 'Фольклор: прочее', 'Folklore', 443, 90, 'folklore');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(453, 453, 'Частушки, прибаутки, потешки', 'Limerick', 443, 100, 'limerick');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(454, 454, 'Фэнтези', 'Fantasy', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(455, 455, 'Боевое фэнтези', 'Fight Fantasy', 454, 10, 'fantasy_fight');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(456, 456, 'Городское фэнтези', 'Fantasy City', 454, 20, 'sf_fantasy_city');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(457, 457, 'Готический роман', 'Gothic Novel', 454, 30, 'gothic_novel');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(458, 458, 'Зарубежное фэнтези', 'Foreign Fantasy', 454, 40, 'foreign_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(459, 459, 'Ироническое фэнтези', 'Ironyc Female Fantasy', 454, 50, 'sf_fantasy_irony');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(460, 460, 'Историческое фэнтези', 'Historical Fantasy', 454, 60, 'historical_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(461, 461, 'Магическое фэнтези', 'Magical Fantasy', 454, 70, 'magician_book');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(462, 462, 'О вампирах', 'Vampire Fantasy', 454, 80, 'vampire_book');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(463, 463, 'О драконах', 'Dragon Fantasy', 454, 90, 'dragon_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(464, 464, 'Приключенческое фэнтези', 'Adventure Fantasy', 454, 100, 'adventure_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(465, 465, 'Сказочная фантастика', 'Fairy Fantasy', 454, 110, 'fairy_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(466, 466, 'Славянское фэнтези', 'Slavic Fantasy', 454, 120, 'russian_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(467, 467, 'Современная сказка', 'Modern Tale', 454, 130, 'modern_tale');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(468, 468, 'Технофэнтези', 'Technofantasy', 454, 140, 'sf_technofantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(469, 469, 'Фэнтези: прочее', 'Fantasy', 454, 150, 'sf_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(470, 470, 'Юмористическое фэнтези', 'Humor Fantasy', 454, 160, 'humor_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(471, 471, 'Хобби и ремесла', 'Hobbies & Crafts', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(472, 472, 'Авто- и мототранспорт, ПДД', 'Auto Regulations', 471, 10, 'auto_regulations');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(473, 473, 'Аквариумистика', 'Aquarium', 471, 20, 'home_aquarium');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(474, 474, 'Альпинизм и скалолазание', 'Mountaineering', 471, 30, 'home_mountain');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(475, 475, 'Виноделие, спиртные напитки', 'Winemaking', 471, 40, 'home_winemaking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(476, 476, 'Животноводство и птицеводство', 'Livestock Farming', 471, 50, 'home_livestock');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(477, 477, 'Изготовление и ремонт мебели', 'Furniture', 471, 60, 'home_furniture');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(478, 478, 'Инвентарь, инструменты', 'Inventory, Instruments', 471, 70, 'home_inventory');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(479, 479, 'Индивидуальное строительство и ремонт', 'Building and Renovation', 471, 80, 'home_building');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(480, 480, 'Книгоделие', 'Bookmaking', 471, 90, 'home_bookmaking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(481, 481, 'Коллекционирование', 'Collecting', 471, 100, 'home_collecting');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(482, 482, 'Моделирование', 'Modelling', 471, 110, 'home_modelling');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(483, 483, 'Морское дело, парусный спорт', 'Maritime Affairs', 471, 120, 'home_marine');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(484, 484, 'Охота и охотоведение', 'Hunt', 471, 130, 'home_hunt');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(485, 485, 'Писательское искусство', 'Writing Art', 471, 140, 'home_writing_art');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(486, 486, 'Пчеловодство', 'Beekeeping', 471, 150, 'home_beekeeping');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(487, 487, 'Работа по дереву', 'Woodwork', 471, 160, 'home_woodwork');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(488, 488, 'Работа по металлу', 'Metal Work', 471, 170, 'home_metalwork');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(489, 489, 'Рукоделие', 'Handiwork', 471, 180, 'home_handiwork');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(490, 490, 'Рыболовство и рыбоводство', 'Fishing', 471, 190, 'home_fishing');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(491, 491, 'Сад и огород', 'Garden', 471, 200, 'home_garden');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(492, 492, 'Сбор и выращивание грибов', 'Mushrooms', 471, 210, 'home_mushrooms');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(493, 493, 'Сделай сам', 'Do It Yourself', 471, 220, 'home_diy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(494, 494, 'Спорт', 'Sports', 471, 230, 'home_sport');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(495, 495, 'Туризм', 'Tourism', 471, 240, 'home_tourism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(496, 496, 'Хобби и ремесла: прочее', 'Hobbies & Crafts Other', 471, 250, 'home_crafts');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(497, 497, 'Цветоводство', 'Floriculture', 471, 260, 'home_floriculture');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(498, 498, 'Юмор', 'Humor', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(499, 499, 'Анекдоты', 'Anecdote', 498, 10, 'humor_anecdote');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(500, 500, 'Байки', 'Tales', 498, 20, 'humor_tales');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(501, 501, 'Пародии, шаржи, эпиграммы', 'Parodies, Cartoons, Epigrams', 498, 30, 'humor_parody');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(502, 502, 'Сатира', 'Satire', 498, 40, 'humor_satire');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(503, 503, 'Юмор: прочее', 'Humor Other', 498, 50, 'humor');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(504, 504, 'Юмористическая проза', 'Humor Prose', 498, 60, 'humor_prose');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(505, 505, 'Юмористические стихи', 'Humor Verses', 498, 70, 'humor_verse');

/* Table data [objects_without_data] Record count: 0 */

/* Table data [params] Record count: 1 */
INSERT INTO [params]([rowid], [id], [name], [value]) VALUES(1, 1, 'version', '7');

/* Commit transaction */
COMMIT;

/* Enable foreign keys */
PRAGMA foreign_keys = 'on';
