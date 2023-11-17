// SPIKE TESTS:
//      Tests how the server handles extreme spikes in traffic
import http from 'k6/http';
import {sleep} from 'k6';

export let options = {
    insecureSkipTLSVerify: true,
    noConnectionReuse: false,
    stages: [
        { duration: '10s', target: 10}, // below normal load
        { duration: '10s', target: 500}, // spike to 500 users
        { duration: '10s', target: 500}, // stay at 500 users for 10 seconds
        { duration: '10s', target: 10}, // scale down. Recovery stage
        { duration: '5s', target: 0} // scale down. Recovery stage
    ]
}

const API_BASE_URL = 'http://localhost:8000';

export default () => {

    http.batch([
        ['GET', `${API_BASE_URL}/`],
        ['GET', `${API_BASE_URL}/about`],
        ['GET', `${API_BASE_URL}/send-files`],
        ['GET', `${API_BASE_URL}/storage`],
        ['GET', `${API_BASE_URL}/form`]
    ]);
    sleep(1);
}