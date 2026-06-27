/* =========================================================
   1. TABLES
   ========================================================= */
DROP TABLE Tickets; DROP TABLE Bookings; DROP TABLE Seats; 
DROP TABLE Shows; DROP TABLE Movies; DROP TABLE Theaters; DROP TABLE Uusers;

CREATE TABLE Movies (movie_id INT PRIMARY KEY, title VARCHAR2(100), duration_mins INT);
CREATE TABLE Uusers (user_id INT PRIMARY KEY, username VARCHAR2(50), email VARCHAR2(100));

CREATE TABLE Theaters (
    theater_id INT PRIMARY KEY, 
    name VARCHAR2(100), 
    location VARCHAR2(100),
    admin_password VARCHAR2(50) DEFAULT 'admin123'
);

CREATE TABLE Shows (
    show_id INT PRIMARY KEY, 
    movie_id INT REFERENCES Movies(movie_id) ON DELETE CASCADE, 
    theater_id INT REFERENCES Theaters(theater_id) ON DELETE CASCADE, 
    show_time TIMESTAMP, 
    price NUMBER(10,2)
);

CREATE TABLE Seats (
    seat_id INT PRIMARY KEY, 
    show_id INT REFERENCES Shows(show_id) ON DELETE CASCADE, 
    seat_number VARCHAR2(10), 
    status INT DEFAULT 0
);

CREATE TABLE Bookings (
    booking_id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, 
    user_id INT REFERENCES Uusers(user_id), 
    show_id INT REFERENCES Shows(show_id) ON DELETE CASCADE, 
    booking_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    total_amount NUMBER(10, 2) -- ADDED BACK TO FIX ORA-00904
);

CREATE TABLE Tickets (
    ticket_id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, 
    booking_id INT REFERENCES Bookings(booking_id) ON DELETE CASCADE, 
    seat_id INT REFERENCES Seats(seat_id) ON DELETE CASCADE
);

/* =========================================================
   2. TRIGGERS & PROCEDURES
   ========================================================= */
CREATE OR REPLACE TRIGGER update_seat_booked AFTER INSERT ON Tickets FOR EACH ROW
BEGIN UPDATE Seats SET status = 1 WHERE seat_id = :NEW.seat_id; END;
/
CREATE OR REPLACE TRIGGER trg_cancel_seat AFTER DELETE ON Tickets FOR EACH ROW
BEGIN UPDATE Seats SET status = 0 WHERE seat_id = :OLD.seat_id; END;
/

CREATE OR REPLACE PROCEDURE add_show_with_seats(
    p_sid IN INT, p_mid IN INT, p_tid IN INT, p_time IN VARCHAR2, p_price IN NUMBER, p_cap IN INT
) AS
BEGIN
    INSERT INTO Shows VALUES (p_sid, p_mid, p_tid, TO_TIMESTAMP(p_time, 'YYYY-MM-DD HH24:MI:SS'), p_price);
    FOR i IN 1..p_cap LOOP
        INSERT INTO Seats (seat_id, show_id, seat_number, status)
        VALUES ((p_sid * 100) + i, p_sid, 'S' || i, 0);
    END LOOP;
    COMMIT;
END;
/

/* =========================================================
   3. DUMMY DATA
   ========================================================= */
INSERT INTO Movies VALUES (1, 'Inception', 148);
INSERT INTO Theaters VALUES (10, 'PVR Mumbai', 'Andheri', 'pvr123');
INSERT INTO Theaters VALUES (20, 'INOX Delhi', 'CP', 'inox123');
INSERT INTO Uusers VALUES (500, 'Rahul', 'rahul@email.com');
EXEC add_show_with_seats(1001, 1, 10, '2024-12-25 18:00:00', 350.00, 15);
COMMIT;
