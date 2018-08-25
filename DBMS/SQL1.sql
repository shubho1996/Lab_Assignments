USE shubho;
create table Employee(
    EMP_CODE char(5),
    EMP_NAME char(20),
    DEPT_CODE char(5),
    DESIG_CODE char(5),
    GENDER char(1),
    ADDRESS char(25),
    CITY char(20),
    STATE char(20),
    PIN char(6),
    BASIC int,
    JN_DT Date,Primary Key(EMP_CODE));

create table DESIGNATION(DESIG_CODE char(5),
DESIG_DESC char(20),
Primary key(DESIG_CODE));
create table DEPARTMENT(DEPT_CODE char(5),
DEPT_NAME char(15),
Primary key(DEPT_CODE));

Insert into DESIGNATION values('DS001','Manager');
Insert into DESIGNATION values('DS002','Excutive');
Insert into DESIGNATION values('DS003','Officer');
Insert into DESIGNATION values('DS004','Clerk');
Insert into DESIGNATION values('DS005','Helper');

Insert into DEPARTMENT values('DP001','Personnel');
Insert into DEPARTMENT values('DP002','Production');
Insert into DEPARTMENT values('DP003','Purchase');
Insert into DEPARTMENT values('DP004','Finance');
Insert into DEPARTMENT values('DP005','Research');

Insert into Employee values('EP001','Shubho','DP003','DS002','M','3305A,Sec-3','FBD','HR','121004',520000,TO_DATE('2001-10-10','YYYY-MM-DD'));
Insert into Employee values('EP002','Shivam','DP001','DS003','M','221,Pocket-1,Sec-9','BLB','PN','121004',250000,TO_DATE('2003-01-11','YYYY-MM-DD'));
Insert into Employee values('EP003','Amitabh','DP005','DS001','M','201,Vaishali App.,Qarth','FBD','WB','121004',420000,TO_DATE('2005-04-15','YYYY-MM-DD'));
Insert into Employee values('EP004','Saptarshi','DP003','DS003','M','43,PoliceLine','Allahbadh','UP','121004',160000,TO_DATE('2005-10-10','YYYY-MM-DD'));

Insert into Employee values('EP005','Kamal',NULL,'DS005','M','23AB,Sec-21','FBD','HR','121034',NULL,TO_DATE('2006-01-06','YYYY-MM-DD'));
Insert into Employee values('EP006','Sayak',NULL,'DS002','M','23AB,Sec-23','FBD','HR','121036',NULL,TO_DATE('2006-01-10','YYYY-MM-DD'));
Insert into Employee values('EP007','Shubhodeep',NULL,'DS002','M','121B,Vashant Kunj','FR','DEL','111036',NULL,TO_DATE('2006-12-10','YYYY-MM-DD'));

Select * from Employee where DEPT_CODE is NULL;

Select * from Employee where BASIC=0;

Select * from Employee where BASIC is NULL;

Select avg(BASIC) AVERGAE from Employee;

Update Employee set BASIC=0 where BASIC is Null;

Select avg(BASIC) AVERGAE2 from Employee;

Delete from Employee where DEPT_CODE is NULL;

Select EMP_Name Name,(BASIC+0.2*BASIC+0.6*BASIC) Net_Pay from Employee;

Select upper(EMP_Name),BASIC from Employee order by DEPT_CODE;

Select * from Employee where JN_DT>TO_DATE('2013-01-01','YYYY-MM-DD');

Select count(EMP_Name) from Employee where EXTRACT(month from JN_DT) = '01';

Select max(BASIC),min(BASIC) from Employee;

Select count(EMP_Name) from Employee where GENDER='F';

Update Employee set CITY=upper(CITY);

Select count(Distinct CITY) from Employee;

Select Employee.*,DEPARTMENT.DEPT_NAME from Employee,DEPARTMENT where Employee.Dept_Code=Department.Dept_Code;