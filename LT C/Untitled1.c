#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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
    int totalSeats;
    int bookedSeats;
    double price;
} Trip;

typedef struct {
    // [LUU Y] ticketId va paymentStatus PHAI NAM O DAY (trong Ticket)
    char ticketId[MAX_STRING];
    int paymentStatus;            // 0: Chua TT, 1: Da TT

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

// ==================================================================================
// --- PHAN 2: CAC HAM HO TRO ---
// ==================================================================================

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
            printf("\t-> Loi: Thong tin khong duoc de trong! Nhap lai.\n");
        } else {
            break;
        }
    } while (1);
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

// --- PHAN 3: CAC CHUC NANG ---

void featureAddTrip() {
    if (currentTripCount >= MAX_TRIPS) { printf("Loi: Full!\n"); return; }
    Trip newTrip;
    printf("\n--- THEM CHUYEN XE ---\n");

    do {
        getValidString("Nhap Ma chuyen xe (ID): ", newTrip.tripId);
        if (isIdDuplicate(newTrip.tripId)) printf("-> Loi: Trung ID!\n");
        else break;
    } while (1);

    getValidString("Diem di: ", newTrip.departure.name);
    getValidString("Diem den: ", newTrip.destination.name);
    getValidString("Ngay (dd/mm/yyyy): ", newTrip.date);

    char buffer[50];
    do {
        printf("Tong so ghe: "); fgets(buffer, 50, stdin);
        newTrip.totalSeats = atoi(buffer);
    } while (newTrip.totalSeats <= 0);

    do {
        printf("Gia ve (VND): "); fgets(buffer, 50, stdin);
        newTrip.price = atof(buffer);
        if (newTrip.price <= 0) printf("-> Loi: Gia > 0.\n");
    } while (newTrip.price <= 0);

    newTrip.bookedSeats = 0;
    tripList[currentTripCount++] = newTrip;
    printf("-> Them moi thanh cong!\n");
}

void featureUpdateTrip() {
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

    getValidString("Diem di moi: ", t->departure.name);
    getValidString("Diem den moi: ", t->destination.name);
    getValidString("Ngay moi: ", t->date);

    char buffer[50];
    int newTotalSeats;
    do {
        printf("Tong so ghe moi (Da dat %d): ", t->bookedSeats);
        fgets(buffer, 50, stdin); newTotalSeats = atoi(buffer);
        if (newTotalSeats < t->bookedSeats) printf("-> Loi: Khong duoc nho hon so ghe da dat!\n");
        else { t->totalSeats = newTotalSeats; break; }
    } while (1);

    do {
        printf("Gia ve moi: "); fgets(buffer, 50, stdin);
        double newPrice = atof(buffer);
        if (newPrice > 0) { t->price = newPrice; break; }
    } while (1);

    printf("-> Cap nhat thanh cong!\n");
}

void featureBookTicket() {
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
    getValidString("SDT: ", newTicket.passengerPhone);

    int seat;
    do {
        printf("Chon ghe (1 - %d): ", t->totalSeats);
        char buf[50]; fgets(buf, 50, stdin); seat = atoi(buf);
        if (seat < 1 || seat > t->totalSeats) printf("-> So ghe sai.\n");
        else if (isSeatBooked(t->tripId, seat)) printf("-> Ghe da co nguoi.\n");
        else { newTicket.seatNumber = seat; break; }
    } while (1);

    newTicket.price = t->price;

    // [LUU Y] Tu dong sinh ma: Ve so 1 -> VE001, Ve so 2 -> VE002
    sprintf(newTicket.ticketId, "VE%03d", currentTicketCount + 1);
    newTicket.paymentStatus = 0;

    ticketList[currentTicketCount++] = newTicket;
    t->bookedSeats++;

    printf("-> DAT VE THANH CONG! Ma ve: %s. Gia: %.0f\n", newTicket.ticketId, newTicket.price);
}

// --- TINH NANG 4: KIEM TRA VE (TIM KIEM THONG MINH) ---
void featureCheckTicketStatus() {
    char inputId[MAX_STRING];
    char searchId[MAX_STRING];

    printf("\n--- KIEM TRA VE ---\n");
    if (currentTicketCount == 0) {
        printf("-> He thong chua co ve nao. Hay Dat ve truoc!\n");
        return;
    }

    // [DEBUG] Hien thi danh sach ve hien co de nguoi dung biet ma nhap
    printf("   [Danh sach ve hien co]:\n");
    for(int k=0; k<currentTicketCount; k++) {
        printf("   -> %s (Khach: %s)\n", ticketList[k].ticketId, ticketList[k].passengerName);
    }
    printf("   -----------------------\n");

    getValidString("Nhap Ma ve (VD: Nhap 1 hoac VE001): ", inputId);

    // [LUU Y] Logic tim kiem thong minh: Nhap 1 -> Tu dong hieu la VE001
    if (isdigit(inputId[0])) {
        int number = atoi(inputId);
        sprintf(searchId, "VE%03d", number);
        printf("-> He thong dang tim kiem ma: %s ...\n", searchId);
    } else {
        strcpy(searchId, inputId);
    }

    int foundIndex = -1;
    for (int i = 0; i < currentTicketCount; i++) {
        if (strcmp(ticketList[i].ticketId, searchId) == 0) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1) {
        printf("-> Loi: Khong tim thay ve co ma: %s\n", searchId);
        return;
    }

    Ticket t = ticketList[foundIndex];
    char tripRoute[MAX_STRING] = "Unknown";

    for (int i = 0; i < currentTripCount; i++) {
        if (strcmp(tripList[i].tripId, t.tripId) == 0){
            sprintf(tripRoute, "%s -> %s", tripList[i].departure.name, tripList[i].destination.name);
            break;
        }
    }

    printf("\n%-6s | %-15s | %-10s | %-30s | %-10s | %-15s | %s\n", "Ma Ve", "Ten", "SDT", "Lo trinh", "Ghe", "Gia", "Trang Thai");
    printf("----------------------------------------------------------------------------------------------------------------------\n");
    printf("%-6s | %-15s | %-10s | %-30s | %-10d | %-15.0f | %s\n",
        t.ticketId, t.passengerName, t.passengerPhone, tripRoute, t.seatNumber, t.price,
        (t.paymentStatus == 1) ? "Da TT" : "Chua TT");
}

//Phan 5: Liet Ke Chuyen Xe

void featureDisplayTrips() {
    printf("\n%-10s | %-15s | %-15s | %-12s | %-8s | %s\n", "ID", "Diem Di", "Diem Den", "Ngay", "Ghe", "Gia Ve");
    printf("--------------------------------------------------------------------------\n");
    for (int i = 0; i < currentTripCount; i++) {
        printf("%-10s | %-15s | %-15s | %-12s | %d / %d | %.0f\n",
            tripList[i].tripId, tripList[i].departure.name, tripList[i].destination.name,
            tripList[i].date, tripList[i].bookedSeats, tripList[i].totalSeats, tripList[i].price);
    }
}

//Phan 6: TT ve




// --- PHAN 4: MAIN ---

int main() {
    int choice;
    char buffer[50];

    do {
        printf("\n\t==========================================================\n");
        printf("\t||             HE THONG QUAN LY BAN VE XE               ||\n");
        printf("\t==========================================================\n");
        printf("\t|  %-54s|\n", "1. Them chuyen xe");
        printf("\t|  %-54s|\n", "2. Cap nhat thong tin chuyen xe");
        printf("\t|  %-54s|\n", "3. Dat ve");
        printf("\t|  %-54s|\n", "4. Kiem tra tinh trang ve");
        printf("\t|  %-54s|\n", "5. Liet ke chuyen xe");
        printf("\t|  %-54s|\n", "0. Thoat chuong trinh");
        printf("\t==========================================================\n");

        printf("\n\t-> Nhap chuc nang: ");
        fgets(buffer, 50, stdin);

        if (buffer[0] == '\n') choice = -1;
        else choice = atoi(buffer);

        switch (choice) {
            case 1: featureAddTrip(); break;
            case 2: featureUpdateTrip(); break;
            case 3: featureBookTicket(); break;
            case 4: featureCheckTicketStatus(); break;
            case 5: featureDisplayTrips(); break;
            case 0: printf("\n\tDang thoat... Tam biet!\n"); break;
            default: printf("\n\tNhap sai! Nhap lai 0-5.\n");
        }

        if (choice != 0) { printf("\n\tAn Enter de quay lai menu..."); getchar(); }

    } while (choice != 0);

    return 0;
}