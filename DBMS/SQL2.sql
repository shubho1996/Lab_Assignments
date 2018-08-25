select min(basic),max(basic),avg(basic),Dept_Code from Employee group by Dept_Code;

select Dept_code, count(Emp_Code) from Employee where Gender='M' group by Dept_code;

select Dept_Code,City, count(*) from Employee group by Dept_Code,City;

select Desig_Code,count(*)from employee group by department,Desig_Code order by count(*);

select Dept_code,sum(basic) as total from employee where gender='M' group by Dept_Code having total>50000 order by total desc;

select a.Emp_Name,b.Desg_Desc from Employee as a,Designation as b where a.Desig_Code=b.Desig_Code;

select p.Emp_Name, q.Desig_Desc,r.dept_Name,p.Basic from Employee p, Designation q,Department r where p.Desig_Code=q.Desig_Code and p.Dept_Code=r.Dept_Code;


select Dept_name from Department where Dept_Code Not IN(select Distinct Dept_Code from Employee);

select Dept_Name from Department where Dept_Code IN(select Distinct Dept_Code from Employee);

select Dept_Name from Department where Dept_Code IN(select Dept_Code from Employee group by Dept_Code having count(*)>10);

select Dept_Code from employee where basic=(select max(basic) from Employee);

select Desig_Desc from Designation where Desig_code in(select Desig_Code from Employee where basic=(select max(Basic) from employee));

select DEPT_CODE,count(*) from employee where Desig_Code in(Select Desig_Code from designation where Desig_Desc='Manager')group by DEPT_CODE ;

select EMP_NAME,BASIC from employee where basic>= all(select distinct Basic  from employee);

select EMP_NAME,BASIC from employee where basic<= all(select distinct BASIC from employee);


select G.Dept_Name as highest_total from (select D.Dept_Name,sum(E.basic) as b from department as D,employee as E where E.DEPT_CODE=D.DEPT_CODE group by E.DEPT_CODE order by b desc limit 1) as F,department as G where F.DEPT_NAME=G.Dept_Name; 

select G.Dept_Name as highest_Average from (select D.Dept_Name,avg(E.basic) as b from department as D,employee as E where E.DEPT_CODE=D.DEPT_CODE group by E.DEPT_CODE order by b desc limit 1) as F,department as G where F.DEPT_NAME=G.Dept_Name; 

select G.Dept_Name as highest_No_of_Employees from (select D.Dept_Name,count(*) as b from department as D,employee as E where E.DEPT_CODE=D.DEPT_CODE group by E.DEPT_CODE order by b desc limit 1) as F,department as G where F.DEPT_NAME=G.Dept_Name; 

Insert into Employee values('EP006','Shubhorere','DP008','DS003','M','3305A,Sec-3','FBD','HR','121004',220000,'2001-10-10');
Insert into Employee values('EP008','horere','DP010','DS003','M','3305A,Sec-3','FBD','HR','121004',220000,'2001-10-10');

delete from employee where DESIG_CODE not in(select DESIG_CODE from designation);

select Emp_Name from Employee as E,(select Dept_Code, avg(Basic) as sal from Employee group by Dept_Code) as T where E.Dept_Code=T.Dept_Code and gender='F' and basic>T.sal;

Select count(*) No_of_Female_Managers from Employee where gender='F' and Desig_Code=(select Desig_Code from designation where DESIG_DESC='Manager');

