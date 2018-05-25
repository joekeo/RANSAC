clear all
fileID = fopen('output.txt','r');
formatSpec = '%f';
data = fscanf(fileID,formatSpec);
fclose(fileID);



A = [data(1) data(3)]; 
B = [data(2) data(4)]; 

hold on
counter = 0;
for iter = 5:3:3004
  if data(iter+2) == 1
    plot(data(iter+1),data(iter),'o', 'Color', 'b')
    counter+1;
  else
    plot(data(iter+1),data(iter),'x', 'Color', 'r')
  end
  
end


plot (B,A)
hold off
