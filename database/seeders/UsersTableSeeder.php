<?php

namespace Database\Seeders;

use Illuminate\Database\Console\Seeds\WithoutModelEvents;
use Illuminate\Database\Seeder;
use Illuminate\Support\Facades\DB;
use Illuminate\Support\Facades\Hash;

class UsersTableSeeder extends Seeder
{
    /**
     * Run the database seeds.
     *
     * @return void
     */
    public function run()
    {
        DB::table('users')->insert([
            'name' => 'Mattia',
            'email' => 'mattiadelfranco@gmail.com',
            'password' => Hash::make('password')
        ]);
    }
}

host     : app-75939402-6f02-4bae-b237-37a5f7aab517-do-user-11397729-0.b.db.ondigitalocean.com
port     : 25060
username : smartlockdb
password : AVNS_VbXZ5rKbc9fmtsr
database : smartlockdb
sslmode  : require
