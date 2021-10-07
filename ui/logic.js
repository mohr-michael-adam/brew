Highcharts.seriesTypes.scatter.prototype.getPointSpline = Highcharts.seriesTypes.spline.prototype.getPointSpline;

var chart;

var brew = window.location.pathname.replace(/^\/|\..*$/g, '');

function requestData() {
    document.title = decodeURIComponent(brew);

    $.getJSON(brew + '.json',
        function (data) {

            var celsius = []
            var gravity = []

            scale = 1

            if (data.length > 500) {
                scale = Math.ceil(data.length / 500);
            }

            for (let i = 0; i < data.length; i+=scale) {
                datetime = Date.parse(data[i]["timestamp"]);
                celsius.push([datetime,data[i]["celsius"]]);
                gravity.push([datetime,data[i]["gravity"]]);
            }

            chart.series[0].setData(gravity);
            chart.series[1].setData(celsius);
        }
    );
}

$(document).ready(function() {
    chart = new Highcharts.Chart({
        chart: {
            renderTo: 'container',
            defaultSeriesType: 'spline',
            marginRight: 130,
            marginBottom: 25,
            events: { load: requestData }
        },
        title: {
            text: decodeURIComponent(brew) + ' Brew Data',
            align: 'left'
        },
        xAxis: {
            type: 'datetime',
            title: {
                text: 'Date'
            }
        },
        yAxis: [{ // Primary yAxis
            labels: {
                format: '{value}°C',
                style: {
                    color: Highcharts.getOptions().colors[2]
                }
            },
            title: {
                text: 'Temperature',
                style: {
                    color: Highcharts.getOptions().colors[2]
                }
            },
            opposite: true
        }, { // Secondary yAxis
            gridLineWidth: 0,
            title: {
                text: 'Gravity',
                style: {
                    color: Highcharts.getOptions().colors[0]
                }
            },
            labels: {
                format: '{value}',
                style: {
                    color: Highcharts.getOptions().colors[0]
                }
            }
        }],
        tooltip: {
            xDateFormat: '%A, %b %m %Y, %H:%M:%S.%L',
            shared: true
        },
        legend: {
            layout: 'vertical',
            align: 'left',
            x: 180,
            verticalAlign: 'top',
            y: 55,
            floating: true,
            backgroundColor:
                Highcharts.defaultOptions.legend.backgroundColor || // theme
                'rgba(255,255,255,0.25)'
        },
        series: [{
            name: 'Gravity',
            type: 'spline',
            yAxis: 1,
            data: [
              [Date.UTC(1971, 5,  7), 77.0],
              [Date.UTC(1971, 5,  8), 97.1],
            ],
            tooltip: {
                valueSuffix: ''
            }
        }, {
            name: 'Temperature',
            type: 'spline',
            data: [
              [Date.UTC(1971, 5,  7), 7.0],
              [Date.UTC(1971, 5,  8), 7.1],
            ],
            tooltip: {
                valueSuffix: ' °C'
            }
        }],
        responsive: {
            rules: [{
                condition: {
                    maxWidth: 500
                },
                chartOptions: {
                    legend: {
                        floating: false,
                        layout: 'horizontal',
                        align: 'center',
                        verticalAlign: 'bottom',
                        x: 0,
                        y: 0
                    },
                    yAxis: [{
                        labels: {
                            align: 'right',
                            x: 0,
                            y: -6
                        },
                        showLastLabel: false
                    }, {
                        labels: {
                            align: 'left',
                            x: 0,
                            y: -6
                        },
                        showLastLabel: false
                    }, {
                        visible: false
                    }]
                }
            }]
        }
    })
});