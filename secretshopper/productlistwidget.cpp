#include "productlistwidget.h"

ProductListWidget::ProductListWidget(QWidget* parent)
    : QWidget(parent) {
    setupUI();
    applyModernStyle();
}

void ProductListWidget::applyModernStyle() {
    // Общий стиль виджета
    this->setStyleSheet(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #f5f7fa, stop:1 #c3cfe2);"
        "}"
    );
}

void ProductListWidget::setupUI() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(15, 15, 15, 15);
    root->setSpacing(12);

    // ─────────────────────────────────────────────────────────────
    // ЗАГОЛОВОК с иконкой корзины и счетчиком
    // ─────────────────────────────────────────────────────────────
    auto* headerFrame = new QFrame();
    headerFrame->setStyleSheet(
        "QFrame {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #667eea, stop:1 #764ba2);"
        "   border-radius: 12px;"
        "   padding: 15px;"
        "}"
    );
    auto* headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(20, 15, 20, 15);

    // Иконка корзины
    m_headerIcon = new QLabel("🛒");
    m_headerIcon->setStyleSheet(
        "QLabel {"
        "   font-size: 36px;"
        "   background: white;"
        "   border-radius: 20px;"
        "   padding: 10px;"
        "   qproperty-alignment: AlignCenter;"
        "}"
    );
    m_headerIcon->setFixedSize(60, 60);
    headerLayout->addWidget(m_headerIcon);

    // Текст заголовка
    auto* titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(5);

    auto* titleLabel = new QLabel("📝 СПИСОК ПОКУПОК");
    titleLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 20px;"
        "   font-weight: bold;"
        "   padding: 0px;"
        "}"
    );
    titleLayout->addWidget(titleLabel);

    auto* subtitleLabel = new QLabel("Добавляйте товары из готовых наборов или вручную");
    subtitleLabel->setStyleSheet(
        "QLabel {"
        "   color: rgba(255, 255, 255, 0.9);"
        "   font-size: 13px;"
        "   padding: 0px;"
        "}"
    );
    titleLayout->addWidget(subtitleLabel);

    headerLayout->addLayout(titleLayout, 1);

    // Счетчик товаров
    auto* countFrame = new QFrame();
    countFrame->setStyleSheet(
        "QFrame {"
        "   background: white;"
        "   border-radius: 10px;"
        "   padding: 10px;"
        "}"
    );
    auto* countLayout = new QVBoxLayout(countFrame);
    countLayout->setAlignment(Qt::AlignCenter);
    countLayout->setContentsMargins(15, 8, 15, 8);

    m_productCountLabel = new QLabel("0");
    m_productCountLabel->setStyleSheet(
        "QLabel {"
        "   color: #667eea;"
        "   font-size: 32px;"
        "   font-weight: bold;"
        "   qproperty-alignment: AlignCenter;"
        "}"
    );
    countLayout->addWidget(m_productCountLabel);

    auto* countTextLabel = new QLabel("товаров");
    countTextLabel->setStyleSheet(
        "QLabel {"
        "   color: #764ba2;"
        "   font-size: 11px;"
        "   font-weight: bold;"
        "   qproperty-alignment: AlignCenter;"
        "}"
    );
    countLayout->addWidget(countTextLabel);

    headerLayout->addWidget(countFrame);

    root->addWidget(headerFrame);

    // ─────────────────────────────────────────────────────────────
    // ОСНОВНАЯ ГРУППА
    // ─────────────────────────────────────────────────────────────
    auto* grp = new QGroupBox();
    grp->setStyleSheet(
        "QGroupBox {"
        "   background: white;"
        "   border: none;"
        "   border-radius: 12px;"
        "   margin-top: 0px;"
        "   padding: 20px;"
        "}"
    );

    auto* gl = new QVBoxLayout(grp);
    gl->setSpacing(12);

    // ─────────────────────────────────────────────────────────────
    // Пресеты
    // ─────────────────────────────────────────────────────────────
    auto* presetsCard = new QFrame();
    presetsCard->setStyleSheet(
        "QFrame {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #f093fb, stop:1 #f5576c);"
        "   border-radius: 10px;"
        "   padding: 15px;"
        "}"
    );
    auto* presetsLayout = new QHBoxLayout(presetsCard);
    presetsLayout->setContentsMargins(15, 12, 15, 12);

    auto* presetLabel = new QLabel("📦 Быстрые наборы:");
    presetLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "}"
    );
    presetsLayout->addWidget(presetLabel);

    m_presets = new QComboBox();
    m_presets->setStyleSheet(
        "QComboBox {"
        "   padding: 10px 15px;"
        "   border: 2px solid rgba(255, 255, 255, 0.5);"
        "   border-radius: 8px;"
        "   background: white;"
        "   font-size: 13px;"
        "   color: #333;"
        "   min-width: 200px;"
        "}"
        "QComboBox:hover {"
        "   border-color: white;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "   width: 35px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   border: 2px solid #f5576c;"
        "   selection-background-color: #f093fb;"
        "   selection-color: white;"
        "}"
    );
    m_presets->addItem("🎯 Выберите готовый набор...");
    m_presets->addItem("🥛 Базовая корзина");
    m_presets->addItem("🍳 Завтрак");
    m_presets->addItem("🍲 Обед");
    m_presets->addItem("🥘 Борщ");
    m_presets->addItem("👶 Детское питание");
    m_presets->addItem("🍦 Молочные продукты");
    m_presets->addItem("🥗 Овощи и фрукты");
    m_presets->addItem("🍖 Мясо и колбасы");
    m_presets->addItem("🍝 Бакалея");
    m_presets->addItem("🛒 Все товары");

    presetsLayout->addWidget(m_presets, 1);

    auto* addPresetBtn = new QPushButton("➕ Добавить");
    addPresetBtn->setStyleSheet(
        "QPushButton {"
        "   background: white;"
        "   color: #f5576c;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 10px 20px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background: #f8f9fa;"
        "   border: 2px solid #f5576c;"
        "}"
        "QPushButton:pressed {"
        "   background: #e9ecef;"
        "}"
    );
    connect(addPresetBtn, &QPushButton::clicked, this, &ProductListWidget::addPreset);
    presetsLayout->addWidget(addPresetBtn);

    gl->addWidget(presetsCard);

    // ─────────────────────────────────────────────────────────────
    // Список товаров
    // ─────────────────────────────────────────────────────────────
    auto* listCard = new QFrame();
    listCard->setStyleSheet(
        "QFrame {"
        "   background: #f8f9fa;"
        "   border: 2px solid #e9ecef;"
        "   border-radius: 10px;"
        "   padding: 10px;"
        "}"
    );
    auto* listLayout = new QVBoxLayout(listCard);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(0);

    m_list = new QListWidget();
    m_list->setDragDropMode(QAbstractItemView::InternalMove);
    m_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_list->setStyleSheet(
        "QListWidget {"
        "   border: none;"
        "   background: transparent;"
        "   font-size: 14px;"
        "   padding: 5px;"
        "}"
        "QListWidget::item {"
        "   padding: 12px 15px;"
        "   border-radius: 6px;"
        "   margin: 3px 0;"
        "   background: white;"
        "   border: 1px solid #dee2e6;"
        "   color: #495057;"
        "}"
        "QListWidget::item:selected {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #667eea, stop:1 #764ba2);"
        "   color: white;"
        "   border: 1px solid #667eea;"
        "}"
        "QListWidget::item:hover:!selected {"
        "   background: #e7f3ff;"
        "   border: 1px solid #667eea;"
        "}"
    );
    connect(m_list->model(), &QAbstractItemModel::rowsInserted,
            this, &ProductListWidget::updateProductCount);
    connect(m_list->model(), &QAbstractItemModel::rowsRemoved,
            this, &ProductListWidget::updateProductCount);

    listLayout->addWidget(m_list);
    gl->addWidget(listCard);

    // ─────────────────────────────────────────────────────────────
    // Поле ввода с автодополнением
    // ─────────────────────────────────────────────────────────────
    auto* inputCard = new QFrame();
    inputCard->setStyleSheet(
        "QFrame {"
        "   background: white;"
        "   border: 2px solid #e9ecef;"
        "   border-radius: 10px;"
        "   padding: 15px;"
        "}"
    );
    auto* inputLayout = new QVBoxLayout(inputCard);
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->setSpacing(10);

    m_input = new QLineEdit();
    m_input->setPlaceholderText("🔍 Введите название товара и нажмите Enter...");
    m_input->setStyleSheet(
        "QLineEdit {"
        "   padding: 12px 15px;"
        "   border: 2px solid #dee2e6;"
        "   border-radius: 8px;"
        "   font-size: 14px;"
        "   background: #f8f9fa;"
        "}"
        "QLineEdit:focus {"
        "   border-color: #667eea;"
        "   background: white;"
        "   border-width: 2px;"
        "}"
    );
    connect(m_input, &QLineEdit::returnPressed, this, &ProductListWidget::addItem);
    connect(m_input, &QLineEdit::textChanged, this, &ProductListWidget::onTextChanged);
    inputLayout->addWidget(m_input);

    // Список подсказок
    m_suggestions = new QListWidget();
    m_suggestions->setMaximumHeight(180);
    m_suggestions->setVisible(false);
    m_suggestions->setStyleSheet(
        "QListWidget {"
        "   border: 2px solid #667eea;"
        "   border-radius: 8px;"
        "   background: white;"
        "   font-size: 13px;"
        "   padding: 5px;"
        "}"
        "QListWidget::item {"
        "   padding: 10px;"
        "   border-radius: 5px;"
        "   margin: 2px 0;"
        "   border: 1px solid transparent;"
        "}"
        "QListWidget::item:hover {"
        "   background: #e7f3ff;"
        "   color: #667eea;"
        "   border: 1px solid #667eea;"
        "}"
        "QListWidget::item:selected {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #667eea, stop:1 #764ba2);"
        "   color: white;"
        "}"
    );
    connect(m_suggestions, &QListWidget::itemClicked,
            this, &ProductListWidget::onSuggestionClicked);
    inputLayout->addWidget(m_suggestions);

    // ─────────────────────────────────────────────────────────────
    // Кнопки управления
    // ─────────────────────────────────────────────────────────────
    auto* buttonRow = new QHBoxLayout();
    buttonRow->setSpacing(10);

    auto* addBtn = new QPushButton("➕ Добавить товар");
    addBtn->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #11998e, stop:1 #38ef7d);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 12px 20px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #0e8278, stop:1 #2ed573);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #0b6b63, stop:1 #26c068);"
        "}"
    );
    connect(addBtn, &QPushButton::clicked, this, &ProductListWidget::addItem);

    auto* delBtn = new QPushButton("🗑 Удалить выбранное");
    delBtn->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #eb3349, stop:1 #f45c43);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 12px 20px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #c92a3a, stop:1 #e04e39);"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #a82330, stop:1 #c94430);"
        "}"
    );
    connect(delBtn, &QPushButton::clicked, this, &ProductListWidget::removeSelected);

    auto* clearBtn = new QPushButton("🗑️ Очистить всё");
    clearBtn->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #c0392b, stop:1 #e74c3c);"
        "   color: white;"
        "   border: 2px solid #922b21;"
        "   border-radius: 8px;"
        "   padding: 12px 20px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #922b21, stop:1 #c0392b);"
        "   border-color: #78281f;"
        "}"
        "QPushButton:pressed {"
        "   background: #78281f;"
        "}"
    );
    connect(clearBtn, &QPushButton::clicked, this, &ProductListWidget::clearAll);

    buttonRow->addWidget(addBtn, 1);
    buttonRow->addWidget(delBtn, 1);
    buttonRow->addWidget(clearBtn, 1);

    inputLayout->addLayout(buttonRow);
    gl->addWidget(inputCard);
    root->addWidget(grp);

    // Инициализация счетчика
    updateProductCount();
}

void ProductListWidget::updateProductCount() {
    int count = m_list->count();
    m_productCountLabel->setText(QString::number(count));

    // Меняем цвет в зависимости от количества
    if (count == 0) {
        m_productCountLabel->setStyleSheet(
            "QLabel {"
            "   color: #95a5a6;"
            "   font-size: 32px;"
            "   font-weight: bold;"
            "   qproperty-alignment: AlignCenter;"
            "}"
        );
    } else if (count < 10) {
        m_productCountLabel->setStyleSheet(
            "QLabel {"
            "   color: #667eea;"
            "   font-size: 32px;"
            "   font-weight: bold;"
            "   qproperty-alignment: AlignCenter;"
            "}"
        );
    } else {
        m_productCountLabel->setStyleSheet(
            "QLabel {"
            "   color: #27ae60;"
            "   font-size: 32px;"
            "   font-weight: bold;"
            "   qproperty-alignment: AlignCenter;"
            "}"
        );
    }
}

void ProductListWidget::onTextChanged(const QString &text) {
    updateSuggestions(text);
}

void ProductListWidget::updateSuggestions(const QString& text) {
    m_suggestions->clear();

    if (text.trimmed().isEmpty()) {
        m_suggestions->setVisible(false);
        return;
    }

    for (const QString& product : m_availableProducts) {
        if (product.contains(text, Qt::CaseInsensitive)) {
            bool alreadyAdded = false;
            for (int i = 0; i < m_list->count(); ++i) {
                if (m_list->item(i)->text() == product) {
                    alreadyAdded = true;
                    break;
                }
            }

            if (!alreadyAdded) {
                m_suggestions->addItem("➕ " + product);
            }
        }
    }

    m_suggestions->setVisible(m_suggestions->count() > 0);
}

void ProductListWidget::onSuggestionClicked(QListWidgetItem* item) {
    if (item) {
        QString productName = item->text().mid(3); // Убираем "➕ "
        m_list->addItem(productName);
        m_input->clear();
        m_suggestions->setVisible(false);
        updateProductCount();
        emit changed();
    }
}

void ProductListWidget::setAvailableProducts(const QStringList& products) {
    m_availableProducts = products;
}

void ProductListWidget::addItem() {
    QString t = m_input->text().trimmed();
    if (t.isEmpty()) return;

    for (int i = 0; i < m_list->count(); ++i) {
        if (m_list->item(i)->text().toLower() == t.toLower()) {
            m_input->clear();
            m_suggestions->setVisible(false);
            return;
        }
    }

    m_list->addItem(t);
    m_input->clear();
    m_suggestions->setVisible(false);
    updateProductCount();
    emit changed();
}

void ProductListWidget::removeSelected() {
    qDeleteAll(m_list->selectedItems());
    updateProductCount();
    emit changed();
}

void ProductListWidget::clearAll() {
    if (m_list->count() > 0) {
        m_list->clear();
        updateProductCount();
        emit changed();
    }
}

void ProductListWidget::addPreset() {
    static const QMap<QString, QStringList> presets = {
        {"🥛 Базовая корзина", {
            "Молоко пастеризованное 3.2% 1л",
            "Хлеб белый нарезной 400г",
            "Хлеб чёрный бородинский 400г",
            "Масло сливочное 82.5% 200г",
            "Яйца куриные С1 10шт",
            "Мука пшеничная высшего сорта 1кг",
            "Сахар белый 1кг",
            "Соль поваренная йодированная 1кг"
        }},
        {"🍳 Завтрак", {
            "Молоко пастеризованное 3.2% 1л",
            "Хлеб белый нарезной 400г",
            "Масло сливочное 82.5% 200г",
            "Яйца куриные С1 10шт",
            "Сыр твёрдый российский 200г",
            "Кефир 2.5% 1л",
            "Творог 5% 200г",
            "Сметана 15% 400г"
        }},
        {"🍲 Обед", {
            "Картофель свежий 1кг",
            "Морковь свежая 1кг",
            "Лук репчатый 1кг",
            "Томаты свежие 1кг",
            "Масло подсолнечное рафинированное 1л",
            "Соль поваренная йодированная 1кг",
            "Сахар белый 1кг",
            "Куриное филе грудки 1кг"
        }},
        {"🥘 Борщ", {
            "Картофель свежий 1кг",
            "Морковь свежая 1кг",
            "Лук репчатый 1кг",
            "Томаты свежие 1кг",
            "Капуста белокочанная 1кг",
            "Свёкла столовая 1кг",
            "Масло подсолнечное рафинированное 1л",
            "Соль поваренная йодированная 1кг",
            "Сахар белый 1кг",
            "Сметана 15% 400г"
        }},
        {"👶 Детское питание", {
            "Пюре детское Агуша яблоко 80г",
            "Пюре детское Агуша груша 80г",
            "Пюре детское Агуша кабачок 80г",
            "Пюре детское ФрутоНяня яблоко-морковь 90г",
            "Пюре детское ФрутоНяня груша-яблоко 90г",
            "Пюре детское ФрутоНяня тыква-яблоко 250г",
            "Каша детская молочная рисовая 200г",
            "Каша детская молочная гречневая 200г",
            "Сок детский Агуша яблоко 200мл",
            "Сок детский ФрутоНяня мультифрукт 200мл"
        }},
        {"🍦 Молочные продукты", {
            "Молоко пастеризованное 3.2% 1л",
            "Кефир 2.5% 1л",
            "Ряженка 4% 1л",
            "Творог 5% 200г",
            "Творог 9% 200г",
            "Сметана 15% 400г",
            "Сметана 20% 400г",
            "Сыр твёрдый российский 200г",
            "Йогурт натуральный 125г",
            "Пломбир Коровка из Кореновки с шоколадной крошкой 15% БЗМЖ 80г",
            "Пломбир Коровка из Кореновки ванильный 15% БЗМЖ 80г",
            "Мороженое магнат шоколадное 70г"
        }},
        {"🥗 Овощи и фрукты", {
            "Картофель свежий 1кг",
            "Морковь свежая 1кг",
            "Лук репчатый 1кг",
            "Капуста белокочанная 1кг",
            "Свёкла столовая 1кг",
            "Огурцы свежие 1кг",
            "Томаты свежие 1кг",
            "Перец болгарский 1кг",
            "Яблоки голден 1кг",
            "Бананы 1кг",
            "Апельсины 1кг",
            "Лимоны 1кг"
        }},
        {"🍖 Мясо и колбасы", {
            "Куриное филе грудки 1кг",
            "Курица целая 1кг",
            "Колбаса варёная докторская 400г",
            "Колбаса варёная молочная 400г",
            "Колбаса полукопчёная охотничья 400г",
            "Сосиски молочные 400г",
            "Сардельки свиные 400г",
            "Ветчина куриная 300г"
        }},
        {"🍝 Бакалея", {
            "Мука пшеничная высшего сорта 1кг",
            "Сахар белый 1кг",
            "Соль поваренная йодированная 1кг",
            "Рис шлифованный длиннозёрный 900г",
            "Гречка ядрица 900г",
            "Пшено 900г",
            "Макароны высший сорт 400г",
            "Масло подсолнечное рафинированное 1л",
            "Чай чёрный байховый 100г",
            "Кофе молотый 100г"
        }},
        {"🛒 Все товары", {
            "Молоко пастеризованное 3.2% 1л",
            "Хлеб белый нарезной 400г",
            "Хлеб чёрный бородинский 400г",
            "Масло сливочное 82.5% 200г",
            "Яйца куриные С1 10шт",
            "Мука пшеничная высшего сорта 1кг",
            "Сахар белый 1кг",
            "Соль поваренная йодированная 1кг",
            "Рис шлифованный длиннозёрный 900г",
            "Гречка ядрица 900г",
            "Масло подсолнечное рафинированное 1л",
            "Макароны высший сорт 400г",
            "Творог 5% 200г",
            "Сметана 15% 400г",
            "Кефир 2.5% 1л",
            "Сыр твёрдый российский 200г",
            "Колбаса варёная докторская 400г",
            "Куриное филе грудки 1кг",
            "Морковь свежая 1кг",
            "Картофель свежий 1кг",
            "Лук репчатый 1кг",
            "Яблоки голден 1кг",
            "Бананы 1кг",
            "Томаты свежие 1кг",
            "Огурцы свежие 1кг",
            "Пюре детское Агуша яблоко 80г",
            "Пюре детское ФрутоНяня яблоко-морковь 90г",
            "Пломбир Коровка из Кореновки с шоколадной крошкой 15% БЗМЖ 80г"
        }}
    };

    QString key = m_presets->currentText();
    if (!presets.contains(key)) return;

    for (const QString& product : presets[key]) {
        bool dup = false;
        for (int i = 0; i < m_list->count(); ++i) {
            if (QString::compare(m_list->item(i)->text(), product, Qt::CaseInsensitive) == 0) {
                dup = true;
                break;
            }
        }
        if (!dup) {
            m_list->addItem(product);
        }
    }

    m_presets->setCurrentIndex(0);
    updateProductCount();
    emit changed();
}

QStringList ProductListWidget::products() const {
    QStringList r;
    for (int i = 0; i < m_list->count(); ++i) {
        r << m_list->item(i)->text();
    }
    return r;
}

void ProductListWidget::setProducts(const QStringList& list) {
    m_list->clear();
    m_list->addItems(list);
    updateProductCount();
    emit changed();
}
