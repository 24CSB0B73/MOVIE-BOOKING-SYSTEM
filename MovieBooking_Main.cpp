#include <iostream>
#include <string>
#include <vector>
#include <occi.h>

using namespace std;
using namespace oracle::occi;

// --- PROTOTYPES ---
void viewShows(Connection* conn);
void viewSeatsVisual(Connection* conn);
void bookTicket(Connection* conn);
void cancelTicket(Connection* conn);
void viewAdminReport(Connection* conn, int theaterId);
void adminMenu(Connection* conn);

int main() {
    try {
        Environment* env = Environment::createEnvironment(Environment::DEFAULT);
        Connection* conn = env->createConnection("YOUR_USERNAME", "YOUR_PASSWORD", "localhost:1521/XEPDB1")

        int choice;
        while (true) {
            cout << "\n===== SWIFT-RESERVE SYSTEM =====\n";
            cout << "1. Browse Shows\n2. View Seat Map\n3. Book Ticket\n4. Cancel Booking\n5. Admin Mode\n6. Exit\nSelection: ";
            if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

            switch (choice) {
            case 1: viewShows(conn); break;
            case 2: viewSeatsVisual(conn); break;
            case 3: bookTicket(conn); break;
            case 4: cancelTicket(conn); break;
            case 5: adminMenu(conn); break;
            case 6: goto cleanup;
            default: cout << "Invalid choice." << endl;
            }
        }
    cleanup:
        env->terminateConnection(conn);
        Environment::terminateEnvironment(env);
    }
    catch (SQLException& e) { cout << "FATAL ERROR: " << e.getMessage() << endl; }
    return 0;
}

void viewShows(Connection* conn) {
    Statement* stmt = conn->createStatement();
    string q = "SELECT s.show_id, m.title, t.name, s.show_time, s.price FROM Shows s JOIN Movies m ON s.movie_id = m.movie_id JOIN Theaters t ON s.theater_id = t.theater_id";
    ResultSet* rs = stmt->executeQuery(q);
    cout << "\n--- AVAILABLE SHOWS ---" << endl;
    while (rs->next()) {
        cout << "ID: " << rs->getInt(1) << " | " << rs->getString(2) << " @ " << rs->getString(3) << " | Rs." << rs->getDouble(5) << endl;
    }
    conn->terminateStatement(stmt);
}

void viewSeatsVisual(Connection* conn) {
    int sid; cout << "Enter Show ID: "; cin >> sid;
    Statement* stmt = conn->createStatement();
    ResultSet* rs = stmt->executeQuery("SELECT seat_id, seat_number, status FROM Seats WHERE show_id = " + to_string(sid) + " ORDER BY seat_id");
    cout << "\n      ========= SCREEN THIS WAY =========" << endl;
    int count = 0;
    while (rs->next()) {
        if (rs->getInt(3) == 1) cout << " [  X  ] ";
        else cout << " [" << rs->getString(2) << ":" << rs->getInt(1) << "] ";
        if (++count % 5 == 0) cout << endl;
    }
    cout << "      ===================================" << endl;
    conn->terminateStatement(stmt);
}

void bookTicket(Connection* conn) {
    int uid, sid, seatid;
    cout << "\n--- BOOKING ---" << endl;
    cout << "User ID: "; cin >> uid; cout << "Show ID: "; cin >> sid; cout << "Seat ID: "; cin >> seatid;
    Statement* stmt = conn->createStatement();
    try {
        // LOCKING FOR CONCURRENCY
        ResultSet* rs = stmt->executeQuery("SELECT status FROM Seats WHERE seat_id = " + to_string(seatid) + " FOR UPDATE");
        if (rs->next() && rs->getInt(1) == 0) {
            stmt->executeUpdate("INSERT INTO Bookings (user_id, show_id, total_amount) SELECT " + to_string(uid) + "," + to_string(sid) + ", price FROM Shows WHERE show_id=" + to_string(sid));
            stmt->executeUpdate("INSERT INTO Tickets (booking_id, seat_id) SELECT MAX(booking_id), " + to_string(seatid) + " FROM Bookings");

            ResultSet* trs = stmt->executeQuery("SELECT MAX(ticket_id) FROM Tickets");
            trs->next(); int tid = trs->getInt(1);
            conn->commit();
            cout << "SUCCESS! Ticket ID: " << tid << endl;
        }
        else cout << "Seat Unavailable!" << endl;
    }
    catch (SQLException& e) { conn->rollback(); cout << "ERROR: " << e.getMessage() << endl; }
    conn->terminateStatement(stmt);
}

void cancelTicket(Connection* conn) {
    int tid; cout << "Enter Ticket ID to cancel: "; cin >> tid;
    Statement* stmt = conn->createStatement();
    try {
        if (stmt->executeUpdate("DELETE FROM Tickets WHERE ticket_id = " + to_string(tid)) > 0) {
            conn->commit(); cout << "Cancelled! Revenue updated." << endl;
        }
        else cout << "Ticket not found." << endl;
    }
    catch (SQLException& e) { conn->rollback(); }
    conn->terminateStatement(stmt);
}

void viewAdminReport(Connection* conn, int theaterId) {
    Statement* stmt = conn->createStatement();
    // Revenue is calculated only from ACTIVE tickets
    string q = "SELECT COUNT(t.ticket_id), SUM(s.price) FROM Tickets t JOIN Bookings b ON t.booking_id = b.booking_id JOIN Shows s ON b.show_id = s.show_id WHERE s.theater_id = " + to_string(theaterId);
    ResultSet* rs = stmt->executeQuery(q);
    if (rs->next()) {
        cout << "\n--- REVENUE REPORT (Theater " << theaterId << ") ---\n";
        cout << "Sold: " << rs->getInt(1) << "\nRevenue: Rs." << rs->getDouble(2) << endl;
    }
    conn->terminateStatement(stmt);
}

void adminMenu(Connection* conn) {
    int tid; string pass;
    cout << "\n--- ADMIN LOGIN ---\nTheater ID: "; cin >> tid; cout << "Password: "; cin >> pass;
    Statement* stmt = conn->createStatement();
    ResultSet* rs = stmt->executeQuery("SELECT name FROM Theaters WHERE theater_id=" + to_string(tid) + " AND admin_password='" + pass + "'");
    if (!rs->next()) { cout << "Invalid Login!" << endl; return; }
    string tname = rs->getString(1);
    while (true) {
        cout << "\nAdmin [" << tname << "]: 1.Add Movie 2.Add Show 3.Cancel Show 4.Report 5.Logout: ";
        int c; cin >> c;
        if (c == 1) {
            int id, d; string t; cout << "ID: "; cin >> id; cout << "Title: "; cin.ignore(); getline(cin, t); cout << "Dur: "; cin >> d;
            stmt->executeUpdate("INSERT INTO Movies VALUES (" + to_string(id) + ", '" + t + "', " + to_string(d) + ")");
            conn->commit(); cout << "Movie Added!" << endl;
        }
        else if (c == 2) {
            int sid, mid, cap; double p; string ts;
            cout << "ShowID: "; cin >> sid; cout << "MovieID: "; cin >> mid; cout << "Time(YYYY-MM-DD HH24:MI:SS): "; cin.ignore(); getline(cin, ts);
            cout << "Price: "; cin >> p; cout << "Capacity: "; cin >> cap;
            Statement* ps = conn->createStatement("BEGIN add_show_with_seats(:1,:2,:3,:4,:5,:6); END;");
            ps->setInt(1, sid); ps->setInt(2, mid); ps->setInt(3, tid); ps->setString(4, ts); ps->setDouble(5, p); ps->setInt(6, cap);
            ps->executeUpdate(); cout << "Show Created!" << endl; conn->terminateStatement(ps);
        }
        else if (c == 3) {
            int sid; cout << "Show ID to delete: "; cin >> sid;
            stmt->executeUpdate("DELETE FROM Shows WHERE show_id=" + to_string(sid) + " AND theater_id=" + to_string(tid));
            conn->commit(); cout << "Show Deleted!" << endl;
        }
        else if (c == 4) viewAdminReport(conn, tid);
        else break;
    }
    conn->terminateStatement(stmt);
}