$(function() {
        $(document).on('click', '#button_forw', function() {
                location.search = '?motorMode=forw&motorVal='+motorVal.value;
                });
        $(document).on('click', '#button_back', function() {
                location.search = '?motorMode=back&motorVal='+motorVal.value;
                });
        $(document).on('click', '#button_right', function() {
                location.search = '?motorMode=right&motorVal='+motorVal.value;
                });
        $(document).on('click', '#button_left', function() {
                location.search = '?motorMode=left&motorVal='+motorVal.value;
                });
        $(document).on('click', '#button_open', function() {
                location.search = '?motorMode=open';
                });
        });

