%CREATEFITS(BAT_VOLTS2,PERCENT2,BAT_VOLTS1,PERCENT1)
%  Create fits.
%
%  Data for 'bat_curve1' fit:
%      X Input : Bat_Volts2
%      Y Output: percent2
%  Data for 'bat_curve2' fit:
%      X Input : Bat_Volts1
%      Y Output: percent1
%  Output:
%      fitresult : a cell-array of fit objects representing the fits.
%      gof : structure array with goodness-of fit info.
%
%  See also FIT, CFIT, SFIT.

%  Auto-generated by MATLAB on 20-May-2015 16:39:45

%% Initialization.

% Initialize arrays to store fits and goodness-of-fit.
fitresult = cell( 2, 1 );
gof = struct( 'sse', cell( 2, 1 ), ...
    'rsquare', [], 'dfe', [], 'adjrsquare', [], 'rmse', [] );

%% Fit: 'bat_curve1'.
[xData, yData] = prepareCurveData( Bat_Volts2, percent2 );

% Set up fittype and options.
ft = fittype( 'poly7' );

% Fit model to data.
[fitresult{1}, gof(1)] = fit( xData, yData, ft, 'Normalize', 'on' );

% Plot fit with data.
figure( 'Name', 'bat_curve1' );
h = plot( fitresult{1}, xData, yData );
legend( h, 'percent2 vs. Bat_Volts2', 'bat_curve1', 'Location', 'NorthEast' );
% Label axes
xlabel Bat_Volts2
ylabel percent2
grid on

%% Fit: 'bat_curve2'.
[xData, yData] = prepareCurveData( Bat_Volts1, percent1 );

% Set up fittype and options.
ft = fittype( 'poly7' );

% Fit model to data.
[fitresult{2}, gof(2)] = fit( xData, yData, ft, 'Normalize', 'on' );

% Plot fit with data.
figure( 'Name', 'bat_curve2' );
h = plot( fitresult{2}, xData, yData );
legend( h, 'percent1 vs. Bat_Volts1', 'bat_curve2', 'Location', 'NorthEast' );
% Label axes
xlabel Bat_Volts1
ylabel percent1
grid on


