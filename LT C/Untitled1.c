#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <conio.h> // Thu vien de dung lenh _getch() (nhap mat khau an)

// --- PHAN 1: CAU TRUC DU LIEU (DATABASE) ---

#define MAX_TRIPS 100
#define MAX_TICKETS 200
#define MAX_STRING 100

typedef struct {
    char name[MAX_STRING];
    char address[MAX_STRING];
} Location;

typedef struct {
    char tripId[MAX_STRING];
    Location departure;
    Location destination;
    char date[MAX_STRING];
    char time[MAX_STRING];
    int totalSeats;
    int bookedSeats;
    double price;
} Trip;

typedef struct {
    char ticketId[MAX_STRING];
    int paymentStatus;            // 0: Chua TT, 1: Da TT
    int isCancelled;              // 0: Binh thuong, 1: Da Huy

    char tripId[MAX_STRING];
    char passengerName[MAX_STRING];
    char passengerPhone[MAX_STRING];
    int seatNumber;
    double price;
} Ticket;

// Global Arrays
Trip tripList[MAX_TRIPS];
int currentTripCount = 0;

Ticket ticketList[MAX_TICKETS];
int currentTicketCount = 0;

// --- PHAN 2: CAC HAM HO TRO ---

// [MOI] Danh sach 63 tinh thanh Viet Nam
const char* PROVINCES[63] = {
    "An Giang", "Ba Ria - Vung Tau", "Bac Giang", "Bac Kan", "Bac Lieu",
    "Bac Ninh", "Ben Tre", "Binh Dinh", "Binh Duong", "Binh Phuoc",
    "Binh Thuan", "Ca Mau", "Can Tho", "Cao Bang", "Da Nang",
    "Dak Lak", "Dak Nong", "Dien Bien", "Dong Nai", "Dong Thap",
    "Gia Lai", "Ha Giang", "Ha Nam", "Ha Noi", "Ha Tinh",
    "Hai Duong", "Hai Phong", "Hau Giang", "Hoa Binh", "Hung Yen",
    "Khanh Hoa", "Kien Giang", "Kon Tum", "Lai Chau", "Lam Dong",
    "Lang Son", "Lao Cai", "Long An", "Nam Dinh", "Nghe An",
    "Ninh Binh", "Ninh Thuan", "Phu Tho", "Phu Yen", "Quang Binh",
    "Quang Nam", "Quang Ngai", "Quang Ninh", "Quang Tri", "Soc Trang",
    "Son La", "Tay Ninh", "Thai Binh", "Thai Nguyen", "Thanh Hoa",
    "Thua Thien Hue", "Tien Giang", "TP Ho Chi Minh", "Tra Vinh", "Tuyen Quang",
    "Vinh Long", "Vinh Phuc", "Yen Bai"
};

int strCmpIgnoreCase(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2))
            return 0;
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

void removeNewline(char* str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
}

void getValidString(char* prompt, char* outStr) {
    do {
        printf("%s", prompt);
        fgets(outStr, MAX_STRING, stdin);
        removeNewline(outStr);
        if (strlen(outStr) == 0) {
            printf("\t-> ERROR: Thong tin khong duoc de trong! Nhap lai.\n");
            continue;
        }

        int hasContent = 0;
        for (int i = 0; i < strlen(outStr); i++) {
            if (outStr[i] != ' ') {
                hasContent = 1;
                break;
            }
        }

        if (hasContent == 0) {
            printf("\t-> ERROR: Thong tin khong duoc de trong! Nhap lai.\n");
        } else {
            break;
        }
    } while (1);
}

void getValidProvince(char* prompt, char* outStr) {
    do {
        printf("%s", prompt);
        fgets(outStr, MAX_STRING, stdin);
        removeNewline(outStr);

        if (strlen(outStr) == 0) {
            printf("\t-> ERROR: Khong duoc de trong!\n");
            continue;
        }

        int found = 0;
        for (int i = 0; i < 63; i++) {
            if (strCmpIgnoreCase(outStr, PROVINCES[i])) {
                strcpy(outStr, PROVINCES[i]);
                found = 1;
                break;
            }
        }

        if (found) {
            break;
        } else {
            printf("\t-> ERROR: Ten tinh/thanh khong ton tai hoac sai chinh ta!\n");
            printf("\t   (Goi y: Ha Noi, TP Ho Chi Minh, Da Nang...)\n");
        }
    } while (1);
}

// Fix nhap bua sdt

void getValidPhone(char* prompt, char* outStr) {
    do {
        printf("%s", prompt);
        fgets(outStr, MAX_STRING, stdin);
        removeNewline(outStr);

        int len = strlen(outStr);  //len: DO dai
        if (len < 10 || len > 11) {
            printf("Error: So dien thoai khong hop le!\n");
            continue;
        }
        // Kiem tra ky tu

        int isAllDigit = 1;
        for (int i = 0; i < len; i++) {
            if (!isdigit(outStr[i])) {
                isAllDigit = 0;
                break;
            }
        }
        if (isAllDigit == 0) {
            printf("Error: So dien thoai khong hop le!\n");
        } else {
            break;
        }
    } while (1);
}

// Fix nhap bua ngay thang, gio
int isValidDate(char* date) {
    if (strlen(date) != 10) return 0; // Kiem tra do dai
    if (date[2] != '/' || date[5] != '/') return 0;

    int day = atoi(date);
    int month = atoi(date + 3);
    int year = atoi(date + 6);

    // Kiem tra thang
    if (month < 1 || month > 12) return 0;
    if (year < 1900) return 0;

    // Kiem tra so ngay trong thang
    int dayInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    // Xu ly nam nhuan
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        dayInMonth[2] = 29;
    }
    if (day < 1 || day > dayInMonth[month]) return 0;

    return 1;
}

//Kiem tra dinh dang ngay gio

int isValidTime(char* time) {
    if (strlen(time) != 5) return 0;
    if (time[2] != ':') return 0;

    int hour = atoi(time);
    int minute = atoi(time + 3);

    if (hour < 0 || hour > 23) {
        return 0;
    }
    if (minute < 0 || minute > 59) {
        return 0;
    }
    return 1;
}

// Check ngay an toan vd: Nhap bua se error dinh dang

void getValidDate(char* prompt, char* outStr) {
    do {
        getValidString(prompt, outStr); // Nhap chuoi truoc
        if (isValidDate(outStr)) {
            break; // Dung logic thi thoat
        } else {
            printf("\t-> ERROR: Ngay khong hop le (dd/mm/yyyy)!\n");
        }
    } while (1);
}

// Ham nhap Gio an toan
void getValidTime(char* prompt, char* outStr) {
    do {
        getValidString(prompt, outStr);
        if (isValidTime(outStr)) {
            break;
        } else {
            printf("\t-> Error: Gio khong hop le (HH:MM tu 00:00 den 23:59)!\n");
        }
    } while (1);
}

int compareDates(char* d1, char* d2) {
    int day1 = atoi(d1);
    int month1 = atoi(d1 + 3);
    int year1 = atoi(d1 + 6);

    int day2 = atoi(d2);
    int month2 = atoi(d2 + 3);
    int year2 = atoi(d2 + 6);

    if (year1 < year2) return -1;
    if (year1 > year2) return 1;
    if (month1 < month2) return -1;
    if (month1 > month2) return 1;
    if (day1 < day2) return -1;
    if (day1 > day2) return 1;
    return 0;
}

// random ma ve
void generateRandomCode(char* buffer) {
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    strcpy(buffer, "TP-");
    int len = 3;
    for (int i = 0; i < 5; i++) {
        buffer[len + i] = charset[rand() % 36];
    }
    buffer[len + 5] = '\0';
}

int isIdDuplicate(char* id) {
    for (int i = 0; i < currentTripCount; i++) {
        if (strcmp(tripList[i].tripId, id) == 0) return 1;
    }
    return 0;
}

int isSeatBooked(char* tripId, int seatNum) {
    for (int i = 0; i < currentTicketCount; i++) {
        if (strcmp(ticketList[i].tripId, tripId) == 0 && ticketList[i].seatNumber == seatNum) {
            return 1;
        }
    }
    return 0;
}

// Nhap mat khau an (Hien thi dau *)
void getPasswordHidden(char* password) {
    int i = 0;
    char ch;
    while (1) {
        ch = _getch(); // Lay ky tu ma khong in ra man hinh
        if (ch == 13) { // Phim Enter
            password[i] = '\0';
            break;
        }
        else if (ch == 8) { // Phim Backspace
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        }
        else {
            password[i] = ch;
            i++;
            printf("*"); // In ra dau *
        }
    }
}

// Kiem tra quyen Admin (User: admin, Pass: admin)
// [DA FIX] Chuan hoa logic check khoang trang va enter
int checkAdminPermission() {
    char username[50];
    char password[50];

    printf("\n--- YEU CAU QUYEN QUAN TRI (ADMIN) ---\n");

    // 1. Nhap tai khoan (Su dung getValidString de chan khoang trang va enter)
    getValidString("Tai khoan: ", username);

    // 2. Nhap mat khau (Co vong lap kiem tra)
    do {
        printf("Mat khau:  ");
        getPasswordHidden(password);
        printf("\n");

        // Kiem tra rong (Enter)
        if (strlen(password) == 0) {
            printf("\t-> ERROR: Mat khau khong duoc de trong! Nhap lai.\n");
            continue;
        }

        // Kiem tra toan khoang trang (Space)
        int hasContent = 0;
        for (int i = 0; i < strlen(password); i++) {
            if (password[i] != ' ') {
                hasContent = 1;
                break;
            }
        }

        if (hasContent == 0) {
            printf("\t-> ERROR: Mat khau khong duoc chua toan khoang trang!\n");
        } else {
            break; // Mat khau hop le ve mat dinh dang
        }
    } while (1);

    // So sanh tai khoan mat khau
    if (strcmp(username, "admin") == 0 && strcmp(password, "admin") == 0) {
        printf("-> Dang nhap thanh cong! Xin chao Admin.\n");
        return 1; // Dung
    } else {
        printf("-> Sai Tai khoan hoac Mat khau! Tu choi truy cap.\n");
        return 0; // Sai
    }
}

//  TAO DU LIEU MAU DE TEST (15 XE, 10 KHACH)
void initDemoData() {
    if (currentTripCount > 0) return;

    // 1. Tao 15 chuyen xe (XE01 -> XE15)
    for (int i = 0; i < 15; i++) {
        Trip t;
        sprintf(t.tripId, "XE%02d", i + 1);
        strcpy(t.departure.name, PROVINCES[rand() % 63]);
        strcpy(t.destination.name, PROVINCES[rand() % 63]);
        while (strcmp(t.departure.name, t.destination.name) == 0) {
             strcpy(t.destination.name, PROVINCES[rand() % 63]);
        }
        sprintf(t.date, "%02d/%02d/2025", 1 + rand() % 28, 1 + rand() % 12);
        sprintf(t.time, "%02d:%02d", 6 + rand() % 14, rand() % 60);
        t.totalSeats = 40;
        t.bookedSeats = 0;
        t.price = 50000 + (rand() % 20) * 10000;
        tripList[i] = t;
    }
    currentTripCount = 15;

    // 2. Tao 10 ve (3 ten dau theo yeu cau)
    const char* fixedNames[] = {"Tan Phong", "Lam Duong", "Khanh Linh"};
    const char* lastNames[] = {"Nguyen", "Tran", "Le", "Pham"};
    const char* firstNames[] = {"Hung", "Dung", "Hoa", "Mai", "Lan"};

    for (int i = 0; i < 10; i++) {
        Ticket tk;
        int tripIdx = rand() % 10;
        Trip *t = &tripList[tripIdx];

        strcpy(tk.tripId, t->tripId);

        if (i < 3) {
            strcpy(tk.passengerName, fixedNames[i]);
        } else {
            sprintf(tk.passengerName, "%s %s", lastNames[rand()%4], firstNames[rand()%5]);
        }

        sprintf(tk.passengerPhone, "09%08d", rand() % 100000000);
        tk.seatNumber = t->bookedSeats + 1;
        tk.price = t->price;

        generateRandomCode(tk.ticketId);
        tk.paymentStatus = rand() % 2; // 0 hoac 1
        tk.isCancelled = 0;

        ticketList[currentTicketCount++] = tk;
        t->bookedSeats++;
    }
}

// --- PHAN 3: CAC CHUC NANG CHINH ---

void featureDisplayTrips() {
    printf("\n%-10s | %-12s | %-12s | %-12s | %-6s | %-6s | %-15s\n",
           "ID", "Diem Di", "Diem Den", "Ngay", "Gio", "Ghe", "Gia Ve");
    printf("----------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < currentTripCount; i++) {
        printf("%-10s | %-12s | %-12s | %-12s | %-6s | %d / %d | %-15.0f\n",
            tripList[i].tripId,
            tripList[i].departure.name,
            tripList[i].destination.name,
            tripList[i].date,
            tripList[i].time,
            tripList[i].bookedSeats,
            tripList[i].totalSeats,
            tripList[i].price
        );
    }
}

//  Hien thi co phan trang cho danh sach nhieu du lieu
void featureDisplayTripsPaginated() { // PHAN TRANG
    if (currentTripCount == 0) {
        printf("-> Danh sach trong!\n");
        return;
    }

    int page = 1;
    int itemsPerPage = 5; // [LUU Y] Hien thi 5 chuyen/trang de test phan trang (co 15 chuyen -> 3 trang)
    int totalPages;

    while (1) {
        // Tinh tong so trang
        totalPages = (currentTripCount + itemsPerPage - 1) / itemsPerPage;

        // Xu ly trang hop le
        if (page < 1) page = 1;
        if (page > totalPages) page = totalPages;

        int start = (page - 1) * itemsPerPage;
        int end = start + itemsPerPage;
        if (end > currentTripCount) end = currentTripCount;

        // In danh sach trang hien tai
        printf("\n================ DANH SACH TRANG [ %d / %d ] ================\n", page, totalPages);
        printf("%-10s | %-12s | %-12s | %-12s | %-6s | %-6s | %-15s\n",
               "ID", "Diem Di", "Diem Den", "Ngay", "Gio", "Ghe", "Gia Ve");
        printf("----------------------------------------------------------------------------------------------\n");

        for (int i = start; i < end; i++) {
            printf("%-10s | %-12s | %-12s | %-12s | %-6s | %d / %d | %-15.0f\n",
                tripList[i].tripId, tripList[i].departure.name, tripList[i].destination.name,
                tripList[i].date, tripList[i].time, tripList[i].bookedSeats, tripList[i].totalSeats, tripList[i].price);
        }
        printf("----------------------------------------------------------------------------------------------\n");
        printf("-> Nhap so trang ban muon xem (Nhap 0 de thoat): ");

        char choice[10];
        fgets(choice, 10, stdin);
        int targetPage = atoi(choice);

        if (targetPage == 0) {
            break;
        } else if (targetPage >= 1 && targetPage <= totalPages) {
            page = targetPage; // Nhay den trang mong muon
        } else {
            printf("-> Loi: Trang %d khong ton tai (Chi co tu 1-%d)!\n", targetPage, totalPages);
            printf("   An Enter de thu lai..."); getchar();
        }
    }
}

void featureAddTrip() { // Them chuyen xe
    // Chi Admin moi duoc vao
    if (checkAdminPermission() == 0) return;

    if (currentTripCount >= MAX_TRIPS) { printf("ERROR: Full!\n"); return; }
    Trip newTrip;
    printf("\n--- THEM CHUYEN XE ---\n");

    do {
        getValidString("Nhap Ma chuyen xe (ID): ", newTrip.tripId);
        if (isIdDuplicate(newTrip.tripId)) printf("-> ERROR: Trung ID!\n");
        else break;
    } while (1);

    getValidProvince("Diem di (Tinh/TP): ", newTrip.departure.name);
    getValidProvince("Diem den (Tinh/TP): ", newTrip.destination.name);

    getValidDate("Ngay khoi hanh (dd/mm/yyyy): ", newTrip.date);
    getValidTime("Gio khoi hanh (HH:MM): ", newTrip.time);

    char buffer[50];
    do {
        printf("Tong so ghe: "); fgets(buffer, 50, stdin);
        newTrip.totalSeats = atoi(buffer);
    } while (newTrip.totalSeats <= 0);

    do {
        printf("Gia ve (VND): ");
        fgets(buffer, 50, stdin);
        newTrip.price = atof(buffer);
        if (newTrip.price <= 0) printf("-> ERROR: Gia > 0.\n");
    } while (newTrip.price <= 0);

    newTrip.bookedSeats = 0;
    tripList[currentTripCount++] = newTrip;
    printf("-> Them moi thanh cong!\n");
}

void featureUpdateTrip() { // Cap nhat chuyen xe
    // [Chi Admin moi duoc vao
    if (checkAdminPermission() == 0) return;

    featureDisplayTrips();
    char id[MAX_STRING];
    printf("\n--- CAP NHAT ---\n");
    getValidString("Nhap ID can sua: ", id);

    int index = -1;
    for (int i = 0; i < currentTripCount; i++) {
        if (strcmp(tripList[i].tripId, id) == 0) { index = i; break; }
    }

    if (index == -1) { printf("-> Khong tim thay!\n"); return; }
    Trip *t = &tripList[index];

    getValidProvince("Diem den moi (Tinh/TP): ", t->destination.name);

    getValidDate("Ngay khoi hanh moi (dd/mm/yyyy): ", t->date);
    getValidTime("Gio khoi hanh moi (HH:MM): ", t->time);

    char buffer[50];
    int newTotalSeats;
    do {
        printf("Tong so ghe moi (Da dat %d): ", t->bookedSeats);
        fgets(buffer, 50, stdin); newTotalSeats = atoi(buffer);
        if (newTotalSeats < t->bookedSeats) printf("-> ERROR: Khong duoc nho hon so ghe da dat!\n");
        else { t->totalSeats = newTotalSeats; break; }
    } while (1);

    do {
        printf("Gia ve moi: ");
        fgets(buffer, 50, stdin);
        double newPrice = atof(buffer);
        if (newPrice > 0) {
            t->price = newPrice;
            break;
        } else {
            printf("-> Loi: Gia phai > 0.\n");
        }
    } while (1);

    printf("-> Cap nhat thanh cong!\n");
}

void featureBookTicket() { // DAT VE Xe
    featureDisplayTrips();
    char inputId[MAX_STRING];
    int foundIndex = -1;

    getValidString("\nNhap ID chuyen xe: ", inputId);
    for (int i = 0; i < currentTripCount; i++) {
        if (strcmp(tripList[i].tripId, inputId) == 0) { foundIndex = i; break; }
    }

    if (foundIndex == -1) { printf("-> Khong tim thay xe!\n"); return; }
    Trip *t = &tripList[foundIndex];

    if (t->bookedSeats >= t->totalSeats) { printf("-> Xe het ghe!\n"); return; }

    Ticket newTicket;
    strcpy(newTicket.tripId, t->tripId);
    getValidString("Ten khach: ", newTicket.passengerName);
    getValidPhone("SDT (10-11 so): ", newTicket.passengerPhone);

    int seat;
    do {
        printf("Chon ghe (1 - %d): ", t->totalSeats);
        char buf[50]; fgets(buf, 50, stdin); seat = atoi(buf);
        if (seat < 1 || seat > t->totalSeats) printf("-> So ghe sai.\n");
        else if (isSeatBooked(t->tripId, seat)) printf("-> Ghe da co nguoi.\n");
        else { newTicket.seatNumber = seat; break; }
    } while (1);

    newTicket.price = t->price;

    generateRandomCode(newTicket.ticketId); // Sinh ma random
    newTicket.paymentStatus = 0;
    newTicket.isCancelled = 0;

    ticketList[currentTicketCount++] = newTicket;
    t->bookedSeats++;

    printf("\n");
    printf("\t*************************************************\n");
    printf("\t* DAT VE THANH CONG!                            *\n");
    printf("\t*-----------------------------------------------*\n");
    printf("\t* MA VE:           %-28s *\n", newTicket.ticketId);
    printf("\t* KHACH HANG:      %-28s *\n", newTicket.passengerName);
    printf("\t* SO TIEN:         %-28.0f *\n", newTicket.price);
    printf("\t* GIO KHOI HANH:   %-28s *\n", t->time);
    printf("\t*************************************************\n");
}

void featureCheckTicketStatus() { // Ktra tinh trang ve
    char inputId[MAX_STRING];

    printf("\n--- KIEM TRA VE ---\n");
    if (currentTicketCount == 0) {
        printf("-> He thong chua co ve nao. Hay Dat ve truoc!\n");
        return;
    }

    printf("   [Danh sach ve hien co]:\n");
    for (int k = 0; k < currentTicketCount; k++) {
        printf("   -> %s (Khach: %s)\n", ticketList[k].ticketId, ticketList[k].passengerName);
    }
    printf("   -----------------------\n");

    getValidString("Nhap chinh xac Ma ve (VD: TP-A82B9): ", inputId);

    int foundIndex = -1;
    for (int i = 0; i < currentTicketCount; i++) {
        // Tim chinh xac ma nhap vao
        if (strcmp(ticketList[i].ticketId, inputId) == 0) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1) {
        printf("-> ERROR: Khong tim thay ve co ma: %s\n", inputId);
        return;
    }

    Ticket t = ticketList[foundIndex];
    char tripRoute[MAX_STRING] = "Unknown";
    char tripTime[MAX_STRING] = "Unknown";

    for (int i = 0; i < currentTripCount; i++) {
        if (strcmp(tripList[i].tripId, t.tripId) == 0) {
            sprintf(tripRoute, "%s -> %s", tripList[i].departure.name, tripList[i].destination.name);
            strcpy(tripTime, tripList[i].time);
            break;
        }
    }

    printf("\n%-10s | %-12s | %-12s | %-15s | %-5s | %-6s | %-15s | %s\n", "Ma Ve", "Ten", "SDT", "Lo trinh", "Ghe", "Gio", "Gia", "Trang Thai");
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("%-10s | %-12s | %-12s | %-15s | %-5d | %-6s | %-15.0f | %s\n",
        t.ticketId, t.passengerName, t.passengerPhone, tripRoute, t.seatNumber, tripTime, t.price,
        (t.paymentStatus == 1) ? "Da TT" : "Chua TT");
}

void featurePayTicket() { // THANH TOAN vé
    char inputId[MAX_STRING];
    printf("\n--- THANH TOAN VE ---\n");
    if (currentTicketCount == 0) {
        printf("-> Chua co ve nao!\n");
        return;
    }
    getValidString("Nhap chinh xac ma ve: ", inputId);

    int foundIndex = -1;
    for (int i = 0; i < currentTicketCount; i++) {
        if (strcmp(ticketList[i].ticketId, inputId) == 0) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1) {
        printf("-> Khong tim thay ve %s !\n", inputId);
        return;
    }

    Ticket *t = &ticketList[foundIndex];
    printf("-> Tim thay: %s (Khach: %s) - Gia: %.0f\n", t->ticketId, t->passengerName, t->price);

    if (t->isCancelled == 1) {
        printf("-> Error: Ve nay da HUY. Khong the TT!\n");
        return;
    }
    if (t->paymentStatus == 1) {
        printf("-> Thong bao: Ve da TT truoc do roi!\n");
        return;
    }

    char confirm[10];
    printf("Xac nhan thanh toan %.0f VND? (y/n): ", t->price);
    fgets(confirm, 10, stdin);

    if (confirm[0] == 'y' || confirm[0] == 'Y') {
        t->paymentStatus = 1;
        printf("-> THANH TOAN THANH CONG!\n");
    } else {
        printf("-> Da huy thao tac\n");
    }
}

// Case 8: Bao cao doanh thu & Thong ke
void featureRevenueReport() { // BAO CAO TINH NAG VA THONG KE
    // [BAO VE] Chi Admin moi duoc vao
    if (checkAdminPermission() == 0) return;

    printf("\n=== BAO CAO DOANH THU & THONG KE ===\n");

    // Kiem tra danh sach rong?
    if (currentTicketCount == 0) {
        printf("-> Thong bao: Khong co du lieu ve de bao cao!\n");
        return;
    }

    printf("1. Tong doanh thu\n");
    printf("2. Thong ke theo chuyen xe\n");
    printf("3. Thong ke theo khoang thoi gian\n");

    int type;
    char buffer[50];
    do {
        printf("-> Chon loai bao cao (1-3): ");
        fgets(buffer, 50, stdin);
        type = atoi(buffer);
        if (type < 1 || type > 3) printf("   Loi: Lua chon khong hop le!\n");
    } while (type < 1 || type > 3);

    // Case 1: Tong doanh thu
    if (type == 1) {
        double totalRevenue = 0;
        int totalPaidTickets = 0;

        for (int i = 0; i < currentTicketCount; i++) {
            if (ticketList[i].paymentStatus == 1 && ticketList[i].isCancelled == 0) {
                totalRevenue += ticketList[i].price;
                totalPaidTickets++;
            }
        }

        printf("\n--- KET QUA BAO CAO TONG HOP ---\n");
        printf("+ Tong doanh thu:      %.0f VND\n", totalRevenue);
        printf("+ Tong ve da thanh toan: %d ve\n", totalPaidTickets);
    }
    // Case 2: Bao cao theo chuyen xe
    else if (type == 2) {
        printf("\n%-10s | %-10s | %-10s | %-5s | %-10s | %s\n",
               "TripID", "Tong Ve", "Da TT", "Huy", "Con HL", "Doanh Thu");
        printf("----------------------------------------------------------------------------\n");

        for (int i = 0; i < currentTripCount; i++) {
            int countTotal = 0, countPaid = 0, countCancel = 0, countActive = 0;
            double tripRevenue = 0;

            for (int k = 0; k < currentTicketCount; k++) {
                if (strcmp(ticketList[k].tripId, tripList[i].tripId) == 0) {
                    countTotal++;
                    if (ticketList[k].isCancelled == 1) {
                        countCancel++;
                    } else {
                        countActive++;
                        if (ticketList[k].paymentStatus == 1) {
                            countPaid++;
                            tripRevenue += ticketList[k].price;
                        }
                    }
                }
            }
            printf("%-10s | %-10d | %-10d | %-5d | %-10d | %.0f\n",
                tripList[i].tripId, countTotal, countPaid, countCancel, countActive, tripRevenue);
        }
    }
    // Case 3: Bao cao theo thoi gian
    else if (type == 3) {
        char fromDate[20], toDate[20];
        printf("\n--- LOC THEO THOI GIAN ---\n");

        do {
            getValidDate("Tu ngay (dd/mm/yyyy): ", fromDate);
            getValidDate("Den ngay (dd/mm/yyyy): ", toDate);

            if (compareDates(fromDate, toDate) > 0) {
                printf("-> Loi: 'Tu ngay' phai nho hon hoac bang 'Den ngay'. Nhap lai!\n");
            } else {
                break;
            }
        } while (1);

        double periodRevenue = 0;
        int countPeriodTotal = 0;
        int countPeriodCancel = 0;
        int countPeriodActive = 0;

        for (int i = 0; i < currentTicketCount; i++) {
            char ticketDate[20] = "";
            for (int t = 0; t < currentTripCount; t++) {
                if (strcmp(tripList[t].tripId, ticketList[i].tripId) == 0) {
                    strcpy(ticketDate, tripList[t].date);
                    break;
                }
            }

            if (compareDates(ticketDate, fromDate) >= 0 && compareDates(ticketDate, toDate) <= 0) {
                countPeriodTotal++;
                if (ticketList[i].isCancelled == 1) {
                    countPeriodCancel++;
                } else {
                    countPeriodActive++;
                    if (ticketList[i].paymentStatus == 1) {
                        periodRevenue += ticketList[i].price;
                    }
                }
            }
        }

        printf("\n--- KET QUA TU %s DEN %s ---\n", fromDate, toDate);
        printf("+ Tong ve trong khoang:  %d\n", countPeriodTotal);
        printf("+ Tong doanh thu thuc:   %.0f VND\n", periodRevenue);
        printf("+ So ve huy:             %d\n", countPeriodCancel);
        printf("+ So ve con hieu luc:    %d\n", countPeriodActive);
    }
}

// --- MAIN ---

int main() {
    srand(time(NULL));

    // Ham Tu dong tao du lieu mau (15 chuyen xe, 10 ve)
    initDemoData();

    int choice;
    char buffer[50];

    do {
        printf("\n\t==========================================================\n");
        printf("\t||             HE THONG QUAN LY BAN VE XE               ||\n");
        printf("\t==========================================================\n");
        printf("\t|  %-54s|\n", "1. Them chuyen xe (Admin)");
        printf("\t|  %-54s|\n", "2. Cap nhat thong tin chuyen xe (Admin)");
        printf("\t|  %-54s|\n", "3. Dat ve");
        printf("\t|  %-54s|\n", "4. Kiem tra tinh trang ve");
        printf("\t|  %-54s|\n", "5. Liet ke chuyen xe");
        printf("\t|  %-54s|\n", "6. Pay Ticket (Thanh toan)");
        printf("\t|  %-54s|\n", "7. Quan ly trang thai ve (KHOA,HUY)");
        printf("\t|  %-54s|\n", "8. Bao cao doanh thu & thong ke ve (Admin)");
        printf("\t|  %-54s|\n", "0. Thoat chuong trinh");
        printf("\t==========================================================\n");

        printf("\n\t-> Nhap chuc nang: ");
        fgets(buffer, 50, stdin);

        if (buffer[0] == '\n') choice = -1;
        else choice = atoi(buffer);

        switch (choice) {
            case 1: featureAddTrip(); break; // Them chuyen xe
            case 2: featureUpdateTrip(); break; // Cap nhat chuyen xe
            case 3: featureBookTicket(); break; // Dat ve xe
            case 4: featureCheckTicketStatus(); break; // Kiem tra ve xe
            case 5: featureDisplayTripsPaginated(); break; // Su dung ham phan trang moi
            case 6: featurePayTicket(); break; // Thanh Toan ve xe
            case 7: printf("Tinh nang dang thu nghiem\n"); break; // [FIX] Them break
            case 8: featureRevenueReport(); break; // Thong ke va doanh thu
            case 0: printf("\n\tDang thoat... Tam biet!\n"); break;
            default: printf("\n\tNhap sai! Nhap lai 0-8.\n");
        }

        if (choice != 0) { printf("\n\tAn Enter de quay lai menu..."); getchar(); }

    } while (choice != 0);

    return 0;
}